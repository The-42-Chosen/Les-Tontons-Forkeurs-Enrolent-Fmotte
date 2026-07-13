<?php
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.php                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:37:19 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/11 16:19:09 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

$method = getenv('REQUEST_METHOD') ?: '';

function print_var($s)
{
    return htmlspecialchars((string) $s, ENT_QUOTES, 'UTF-8');
}

$count = isset($_COOKIE['hits_php']) ? ((int) $_COOKIE['hits_php']) + 1 : 1;
setcookie('hits_php', (string) $count, 0, '/cgi.php');

header('Content-Type: text/html');

echo "<!DOCTYPE html>\n";
echo "<html lang=\"fr\">\n";
echo "<head><meta charset=\"utf-8\"><title>CGI Webserv</title></head>\n";
echo "<style>*{padding: 0;margin: 0;background-color: bisque;}.container{padding: 10px;}body{text-align: center;}h1{color: goldenrod;padding: 10px;}h2{color: maroon;padding: 10px;}</style>\n";
echo "<body>\n";
echo "<h1>CGI Webserv (PHP)</h1>\n";
echo "<h2> Welcome to the Fabulous CGI PHP of Minicube & Rico</h2>\n";
echo "<h2>Visites (PHP) : " . print_var($count) . "</h2>\n";
echo "<p><strong>Method :</strong> " . print_var($method) . "</p>\n";

function print_params($params)
{
    if (empty($params)) {
        echo "<p>No parameters</p>\n";
        return;
    }
    echo "<ul>\n";
    ksort($params);
    foreach ($params as $key => $val) {
        echo "  <li><strong>" . print_var($key) . "</strong> = " . print_var($val) . "</li>\n";
    }
    echo "</ul>\n";
}

echo "<h2>Queries (GET)</h2>\n";
print_params($_GET);
echo "<h2>Body (POST)</h2>\n";
print_params($_POST);

echo "<hr>\n";
echo "<div class=\"container\">\n";
echo "<h2>Test GET /cgi.php</h2>\n";
echo "<form method=\"GET\" action=\"/cgi.php\">\n";
echo "<label for=\"get-msg\">Message :</label>\n";
echo "<input id=\"get-msg\" type=\"text\" name=\"message\">\n";
echo "<button type=\"submit\">Send (GET)</button>\n";
echo "</form>\n";
echo "</div>\n";
echo "<div class=\"container\">\n";
echo "<h2>Test POST /cgi.php</h2>\n";
echo "<form method=\"POST\" action=\"/cgi.php\" enctype=\"application/x-www-form-urlencoded\">\n";
echo "<label for=\"post-msg\">Message :</label>\n";
echo "<input id=\"post-msg\" type=\"text\" name=\"message\">\n";
echo "<button type=\"submit\">Send (POST)</button>\n";
echo "</form>\n";
echo "</div>\n";
echo "<hr>\n";
echo "<p><em>Server: " . print_var(getenv('SERVER_NAME')) . ":" . print_var(getenv('SERVER_PORT'))
    . " | Protocol: " . print_var(getenv('SERVER_PROTOCOL')) . "</em></p>\n";
echo "</body>\n</html>\n";
