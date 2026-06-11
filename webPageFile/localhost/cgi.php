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
$params = ($method === 'POST') ? $_POST : $_GET;

function esc($s)
{
    return htmlspecialchars((string) $s, ENT_QUOTES, 'UTF-8');
}

header('Content-Type: text/html');

echo "<!DOCTYPE html>\n";
echo "<html lang=\"fr\">\n";
echo "<head><meta charset=\"utf-8\"><title>CGI Webserv</title></head>\n";
echo "<body>\n";
echo "<h1>CGI Webserv (PHP)</h1>\n";
echo "<h2> Welcome to the Fabulous CGI PHP of Minicub & Rico</p>\n";
echo "<p><strong>Method :</strong> " . esc($method) . "</p>\n";

if (empty($params)) {
    echo "<p>Aucun parametre recu.</p>\n";
} else {
    echo "<h2>Parametres</h2>\n<ul>\n";
    ksort($params);
    foreach ($params as $key => $val) {
        echo "  <li><strong>" . esc($key) . "</strong> = " . esc($val) . "</li>\n";
    }
    echo "</ul>\n";
}

echo "<hr>\n";
echo "<p><em>Server: " . esc(getenv('SERVER_NAME')) . ":" . esc(getenv('SERVER_PORT'))
    . " | Protocol: " . esc(getenv('SERVER_PROTOCOL')) . "</em></p>\n";
echo "</body>\n</html>\n";
