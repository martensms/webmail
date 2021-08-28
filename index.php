<?php
$flush=microtime();
$query=explode(":",$_SERVER['QUERY_STRING']);
require_once('obj/ec_profiles.inc');
require_once('obj/uw_functions.inc');
require_once('obj/uw_config.inc');
require_once('obj/uw_imap.inc');
require_once('obj/uw_smtp.inc');
require_once('obj/uw_message_show.inc');

$cfg['beta']=false;
$cfg['mode']['nocss']=($_COOKIE['nocss']?$_COOKIE['nocss']:false);
$cfg['mode']['mini']=false;
$cfg['tpl']['var']['version'] = "2.0 RC1";

$cfg['tpl']['url']            = "./tpl";
$cfg['tpl']['css']['name']    = "Light Blue";
$cfg['tpl']['css']['url']     = "./tpl/light_blue";
$cfg['tpl']['cnt']['toolbar'] = ""; # prevent viewing a toolbar if nothing works.
$cfg['tpl']['cnt']['css']     = "@import url('".$cfg['tpl']['css']['url']."/style.css');";

# - needs $cfg['tpl']['css']['url']."/mime"! - #
require_once('obj/uw_mime.inc');
#print_r($cfg['mime']);

if((is_array($_COOKIE)) || $query[0]=="check"){
 $obj['imap']=@new uw_imap($_COOKIE['username'],$_COOKIE['password'],$cfg);
}
# - - - used variables - - - #
 $input = $_POST['input'];
# / #

switch($query[0]){
 default:
 case "check":
 case "login":
 case "logout":
  $cfg['tpl']['var']['title'] = "(Re)Configure interface settings";
  if($cfg['mode']['mini']){
   $cfg['tpl']['file']        = "login2.xhtml";
  }else{
   $cfg['tpl']['file']        = "login.xhtml";
  }
  $cfg['tpl']['cnt']['css']  .= "@import url('".$cfg['tpl']['css']['url']."/login.css');";
  include "./inc/login.php";
 break;
 case "folders":
  $cfg['tpl']['var']['title'] = "Folder Management";
  $cfg['tpl']['file']         = "folders.xhtml";
  $cfg['tpl']['cnt']['css']  .= "@import url('".$cfg['tpl']['css']['url']."/folders.css');";
  include "./inc/folders.php";
 break;
 case "compose":
  $cfg['tpl']['var']['title'] = "Compose Message";
  $cfg['tpl']['file']         = "compose.xhtml";
  $cfg['tpl']['cnt']['css']  .= "@import url('".$cfg['tpl']['css']['url']."/compose.css');";
  include "./inc/compose.php";
 break;
 case "mailbox":
  $cfg['tpl']['var']['title'] = "Mailbox";
  $cfg['tpl']['file']         = "mailbox.xhtml";
  $cfg['tpl']['cnt']['css']  .= "@import url('".$cfg['tpl']['css']['url']."/mailbox.css');";
  include "./inc/mailbox.php";
 break;
 case "message":
 case "message_iframe":
  $cfg['tpl']['var']['title'] = "Read Message";
  $cfg['tpl']['file']         = "message.xhtml";
  $cfg['tpl']['cnt']['css']  .= "@import url('".$cfg['tpl']['css']['url']."/message.css');";
  include "./inc/message.php";
 break;
 case "popup":
  $cfg['tpl']['var']['title'] = "Header &amp; Attachment Details";
  $cfg['tpl']['file']         = "popup.xhtml";
  include "./inc/popup.php";
 break;
 case "uspace":
  $cfg['tpl']['var']['title']    = "uSpace";
  $cfg['tpl']['var']['jscripts'] = $cfg['tpl']['url']."/uspace.js";
  $cfg['tpl']['cnt']['css']     .= "@import url('".$cfg['tpl']['css']['url']."/uspace.css');";
  include "./inc/uspace.php";
 break;
 break;
 case "nocss":
  setcookie("nocss","true",time()+3600);
  header("Location: ?index");
 break;
 case "css":
  setcookie("nocss","",time()-3600);
  header("Location: ?index");
 break;
}

if($query[0]!="message_iframe" && !preg_match("~iframe~Uis",$query[2]) && $cfg['beta']){
 $cfg['tpl']['cnt']['toolbar'] = "<div class=\"beta\">Unstable &amp; unreleased version - Please click carefully!</div>". $cfg['tpl']['cnt']['toolbar'];
 $cfg['tpl']['cnt']['trace']   = implode('',file("".$cfg['tpl']['url']."/trace.js"));
 $flush = round((microtime()-$flush),4);
 $flush = ($flush<0)?($flush * -1):$flush;
 $cfg['tpl']['cnt']['trace']  .= "\n<div class=\"error\">";
 $cfg['tpl']['cnt']['trace']  .= "Created in ".$flush."sec";
 if($_COOKIE['login']){
  $cfg['tpl']['cnt']['trace']  .= "<br />Login expires in ".(time() - $_COOKIE['login'])* -1 ."sec";
 }
 $cfg['tpl']['cnt']['trace']  .= "</div>";
}else{
 $cfg['tpl']['cnt']['trace'] = "";
}

if(preg_match("~(Windows CE; IEMobile)~Uis",$_SERVER['HTTP_USER_AGENT'])){
 $cfg['tpl']['cnt']['css']   .= "@import url('".$cfg['tpl']['url']."/iemobile.css');";
}
if($cfg['mode']['nocss']){
 $cfg['tpl']['cnt']['css'] = "@import url('".$cfg['tpl']['url']."/handy.css');";
}

$tpl=implode('',file($cfg['tpl']['url']."/".$cfg['tpl']['file']));
$tpl=preg_replace("~{VAR:CSS_TITLE}~Uis", $cfg['tpl']['css']['name'], $tpl);

if(is_array($cfg['tpl']['cnt'])){
 foreach($cfg['tpl']['cnt'] as $name=>$value){ $tpl=preg_replace("~{CNT:".$name."}~Uis", $value, $tpl); }
}
foreach($cfg['tpl']['var'] as $name=>$value){ $tpl=preg_replace("~{VAR:".$name."}~Uis", $value, $tpl); }
$tpl=preg_replace("~{CFG:username}~Uis", isset($_COOKIE['username'])?$_COOKIE['username']:"", $tpl);
foreach($cfg as $name=>$value){
 if(!is_array($value)){ $tpl=preg_replace("~{CFG:$name}~Uis", $value, $tpl); }
}
$tpl=preg_replace("~{VAR:ERROR}~Uis", $_COOKIE['error'], $tpl);

# - - - compress it? - - - #
#$tpl=preg_replace("~\n~Uis","",$tpl);
#print_r($cfg['tpl']);

#if($query[0]!="about" && $query[0]!="cache" && $query[0]!="logout" && $query[0]!="login"){
# header("Status: 200");
# header("Content-Encoding: gzip");
# echo gzencode($tpl);
#}else{
 echo $tpl;
#}
?>
