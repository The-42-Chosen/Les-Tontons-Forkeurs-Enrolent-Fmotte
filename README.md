*This project has been created as part of the 42 curriculum by erpascua, fmotte.*

# Webserv

## Description

**Webserv** is a non-blocking HTTP/1.1 server written from scratch in **C++98**, without any
external library. The goal of the project is to understand how a real web server works by
re-implementing one: parsing an NGINX-like configuration file, accepting connections,
parsing raw HTTP requests, serving static files, executing CGI scripts, and returning
well-formed HTTP responses — all from a single process, single thread, using **`epoll`**
for I/O multiplexing.

The server never blocks: every socket (listening sockets, client sockets and CGI pipes) is
registered in the same `epoll` instance and handled through an event loop. A request is
read incrementally, parsed, routed to the matching `server` block and `location` block,
handled by the right method, and the response is only written when the kernel reports the
socket as writable.

Architecture overview:

| Module | Role |
| --- | --- |
| `src/core` | `main`, event loop (`epoll`), `Client` lifecycle, connection utilities, exceptions |
| `src/config` | Tokenizer and parser of the configuration file (`Server`, `Location`) |
| `src/http` | `HttpRequest` / `HttpResponse`, headers, body (incl. chunked), cookies |
| `src/request` | Request abstraction: `StaticRequest` vs `CGIRequest`, path resolution |
| `src/method` | `GET`, `POST`, `DELETE`, `HEAD` handlers |
| `src/response` | Response builders: correct, error, redirection |

## Features

- HTTP/1.1 server, **single process / single thread**, fully non-blocking (`epoll` + `EPOLLIN` / `EPOLLOUT`)
- Multiple `server` blocks, multiple `listen` directives per server, **virtual hosts** resolved via the `Host` header
- Methods: **GET**, **POST**, **DELETE**, **HEAD**, restricted per location
- Static file serving, `index` files and **directory listing** (`autoindex`)
- **CGI** execution (`.py` via `/usr/bin/python3`, `.php` via `/usr/bin/php-cgi`), with the body piped to the script's stdin and its output read asynchronously
- File **upload** through POST
- Request body via `Content-Length` **and** `Transfer-Encoding: chunked`
- `client_max_body_size` enforcement (server-wide and per location)
- Custom `error_page`, and HTTP **redirections / static returns** through `return`
- **Cookie** support (`Set-Cookie`, `Max-Age`, `Path`, `HttpOnly`)
- Keep-alive connections and graceful shutdown on `SIGINT`

## Instructions

### Requirements

- A C++ compiler supporting **C++98** (`c++` / `g++` / `clang++`)
- `make`
- Linux (the server relies on `epoll`)
- Optional: `python3` and `php-cgi` for CGI scripts, `python3` + `siege` for the test suite

### Compilation

```bash
make          # builds ./webserv
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # full rebuild
```

Compilation flags: `-Wall -Wextra -Werror -std=c++98`.

### Execution

The server takes **exactly one argument**: the path to a configuration file.

```bash
./webserv doc/config_file
```

Shortcut:

```bash
make serv     # rebuilds and runs ./webserv doc/config_file
```

Then open <http://localhost:8080> in a browser, or test from the terminal:

```bash
curl -v http://localhost:8080/
curl -X POST --data-binary @file.txt http://localhost:8080/upload/file.txt
curl -X DELETE http://localhost:8080/upload/file.txt
curl -I http://localhost:8080/            # HEAD
printf 'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n' | nc -C 127.0.0.1 8080
```

Stop the server with `Ctrl+C` (`SIGINT`) — it closes every open fd before exiting.

### Configuration file

The syntax is inspired by NGINX. Supported directives:

| Directive | Context | Description |
| --- | --- | --- |
| `listen <ip:port>` / `listen <port>` | server | Address(es) the server binds to |
| `server_name <name...>` | server | Virtual host names matched against `Host` |
| `root <path>` | server, location | Root directory used to resolve the URI |
| `index <file...>` | server, location | Files served when the URI is a directory |
| `autoindex on\|off` | server, location | Enables the directory listing |
| `allowed_methods <GET POST DELETE HEAD>` | location | Methods accepted on this route |
| `client_max_body_size <bytes>` | server, location | Maximum accepted body size |
| `error_page <code> <path>` | server, location | Custom error page |
| `return <code> [url\|"text"]` | server, location | Redirection or immediate response |

Example:

```nginx
server {
    listen 0.0.0.0:8080;
    server_name localhost mysite.com;

    root ./webPageFile/localhost;
    index index.html;
    client_max_body_size 4;

    error_page 404 ./webPageFile/errors/404.html;

    location /images {
        root ./webPageFile;
        autoindex on;
    }

    location /upload {
        allowed_methods GET POST;
        client_max_body_size 5000000;
    }

    location /google {
        return 301 https://www.google.com/;
    }
}
```

A complete, commented example is available in [`doc/config_file`](doc/config_file).

## Technical choices

- **`epoll` over `poll`/`select`**: a single `epoll` instance owns *every* file descriptor —
  listening sockets, client sockets and both ends of the CGI pipes. Each fd carries an
  `EventData` structure so the event loop knows what it is looking at without scanning any
  array. `EPOLLOUT` is only armed once a response is ready, so we never busy-wait on write.
- **Polymorphic request handling**: `ARequest` is specialised into `StaticRequest` and
  `CGIRequest`; the CGI variant is driven by the event loop instead of blocking on
  `read()`/`waitpid()`, which keeps the server responsive while a script runs.
- **Abstract method / response hierarchies** (`AMethod`, `AResponse`) so adding a method or a
  response family does not touch the event loop.
- **Two-stage configuration parsing**: the file is first tokenized (`tokenizeString`), then
  consumed by `Server` / `Location` which validate directives and reject duplicates or
  missing mandatory ones through dedicated exceptions.

## Resources

### HTTP & RFCs

- [RFC 9110 — HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110.html)
- [RFC 9112 — HTTP/1.1 message syntax](https://www.rfc-editor.org/rfc/rfc9112.html)
- [RFC 3875 — The Common Gateway Interface (CGI) Version 1.1](https://www.rfc-editor.org/rfc/rfc3875.html)
- [RFC 6265 — HTTP State Management Mechanism (Cookies)](https://www.rfc-editor.org/rfc/rfc6265.html)
- [MDN — HTTP reference](https://developer.mozilla.org/en-US/docs/Web/HTTP)

### Server & network programming

- [NGINX documentation](https://nginx.org/en/docs/) — the reference for the configuration syntax
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- `man 7 epoll`, `man 2 epoll_ctl`, `man 2 socket`, `man 2 execve`, `man 2 pipe`
- [The C10K problem](http://www.kegel.com/c10k.html)
- [Siege](https://www.joedog.org/siege-home/) — used for the stress tests

### Use of AI

AI (Claude, through Claude Code) was used as an assistant, never as a substitute for
writing and understanding the code. Concretely:

- **Debugging and code review**: explaining Valgrind reports, tracking down memory leaks and
  invalid reads, and reviewing the event-loop / client-lifecycle code for edge cases such as
  pipelined requests, premature disconnections and dangling `epoll` registrations.
- **Understanding the specification**: clarifying the RFC wording on chunked transfer
  encoding, `Content-Length` vs `Transfer-Encoding` precedence, and CGI environment
  variables, in order to decide which status code to return in ambiguous cases.
- **Documentation**: drafting this README from the actual source tree.

Every suggestion was read, tested and adapted by hand; the architecture, the parsing, the
event loop and the HTTP logic are our own design.
