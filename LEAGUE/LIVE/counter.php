<?php
  include './counter_conf.php';
  mysql_connect($host, $username, $password);
  mysql_select_db($database);
  $result = mysql_query('SELECT value FROM ' . $table . ' WHERE name = "' . $name . '"');
  if ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
    $value = $line['value'];
  } else {
    mysql_query('INSERT INTO ' . $table . ' (name, value) VALUES ("' . $name . '", 0)');
    $value = 0;
  }

  $cookie_name = 'counter_' . $name . '_date';
  $date_str = date('Y-m-d');
  if (!isset($_COOKIE[$cookie_name]) || $_COOKIE[$cookie_name] != $date_str) {
    setcookie($cookie_name, $date_str, time() + 86400);
    $value ++;
    mysql_query('UPDATE ' . $table . ' SET value = value + 1 WHERE name = "' . $name . '"');
  }
  mysql_close();
  echo 'document.write(' . $value .')';
?>