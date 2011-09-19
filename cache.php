<?php
$age=intval($_REQUEST['age']);
$key=$_REQUEST['key'];
$sex=$_REQUEST['sex'];
$fp=@join('',@file('http://www.emkdesign.com/bin/wma2010?age='.$age.'&key='.$key.'&sex='.$sex));
echo $fp;
?>
