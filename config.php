<?php
    // config.php
    // Credenciales
    $dbhost = "localhost";
    $dbuser = "root";
    $dbpass = "contrase�a";
    $dbname = "meteo";
    // Conexión con la base de datos
    $con = mysqli_connect($dbhost, $dbuser, $dbpass, $dbname);
?>