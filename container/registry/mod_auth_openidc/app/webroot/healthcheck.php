<?php
header('Content-Type: application/json; charset=utf-8');

$extensions = get_loaded_extensions();
$healthchecks = [
  'php' => [
    'engine' => 'ok',
    //'extension_list' => $extensions,
    'extensions' => [
      'json' => in_array('json', $extensions),
      'ldap' => in_array('ldap', $extensions),
      'pdo_mysql' => in_array('pdo_mysql', $extensions),
      'mysqli' => in_array('mysqli', $extensions),
      'pdo_pgsql' => in_array('pdo_pgsql', $extensions),
      'pgsql' => in_array('pgsql', $extensions),
      'opcache' => in_array('opcache', $extensions),
    ],
  ],
];

//echo '{"health":"ok"}';
echo json_encode($healthchecks);
