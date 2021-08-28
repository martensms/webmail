<?php
$cfg['tpl']['cnt']['logout_btn'] = "";
$cfg['tpl']['var']['localhost']  = $_SERVER['HTTP_HOST'];
$cfg['tpl']['cnt']['jscripts']   = "";

switch($query[0]){
 default:
 case "check":
  if($obj['imap']->check_login(&$_COOKIE['error'])){
   $_COOKIE['error']="Settings seem to work fine.";
   $cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
<a href=\"?config\">Index</a> |
<a href=\"?mailbox\">Mailbox</a> |
<a href=\"?folders\">Folders</a> |
<a href=\"?uspace\">uSpace</a><sup>New!</sup> |
<a href=\"?compose\">Compose</a> |
</div>";
   $cfg['tpl']['cnt']['logout_btn']="<input type=\"button\" value=\"Logout\" onclick=\"document.location.href='?logout';\" />";
  }else{
   $_COOKIE['error']=imap_last_error();
  }
 break;
 case "login":
  if(is_array($input) && strlen($input['username'])>=3){
   if(isset($input['advcfg'])){
    setcookie("advcfg",$input['advcfg'],time()+3600);
    if($input['advcfg']=="manual"){
     foreach($input as $name=>$value){
      if($name=="flags"){
       foreach($value as $do=>$what){
        if($do=="crypt"){ $flags.="/".$what;
        }elseif($do=="service"){ $flags.="/service=".$what; }
       }
       setcookie("imap_flags",$flags,time()+3600);
      }
      elseif($name!="profile"){
       setcookie($name,$value,time()+3600);
      }
     }
    }elseif($input['advcfg']=="profile"){
      setcookie("advcfg","profile".$input['profile'],time()+3600);
    }elseif($input['advcfg']=="xmlfile"){
      setcookie("advcfg","xmlfile",time()+3600);
      setcookie("url",$input['url'],time()+3600);
    }
   }
   setcookie("username",$input['username'],time()+3600);
   setcookie("password",$input['password'],time()+3600);
   setcookie("login",time()+3600,time()+3600);
  }
  header("Location: ./?check");
 break;
 case "logout":
  foreach($_COOKIE as $name=>$value){
   setcookie($name,"",time()-3600);
  }
  header("Location: ./");
 break;
}

if(is_array($pfs) && !$cfg['mode']['mini']){
 $cfg['tpl']['cnt']['jscripts']="hints = new Array();
 examples = new Array();
 function example(id){
  document.getElementById('username').value=examples[id];
 }
 function hint(id){
  document.getElementById('hint').innerHTML=hints[id] + '\<br \/>\<br \/>\<a href=\"javascript:example('+id+');\">Click to show example above\<\/a>';
  if(document.getElementById('username').value.length==0){
   example(id);
  }
 }
";
 foreach($pfs as $ID=>$array){
 $cfg['tpl']['cnt']['profiles'].="
    <option value=\"".$ID."\" onclick=\"javascript:hint('".$ID."');\">".$array['provider']."</option>";
 $cfg['tpl']['cnt']['jscripts'].="  hints[".$ID."]=\"".$array['hint']."\";
  examples[".$ID."]=\"".($array['example']?$array['example']:$array['hint'])."\";
";
 }
}

if(!$cfg['mode']['mini']){
$cfg['tpl']['cnt']['jscripts'].="
 function hide(id){
  if(id=='pfs'){ document.getElementById('hint').style.display='none'; }
  document.getElementById(id).style.display='none';
 }
 function unhide(id){
  document.getElementById(id).style.display='block';
 }";
}

if($cfg['mode']['nocss']){
 $cfg['tpl']['cnt']['css_btn']="<input type=\"button\" value=\"on\" onclick=\"document.location.href='?css';\" />";
}else{
 $cfg['tpl']['cnt']['css_btn']="<input type=\"button\" value=\"off\" onclick=\"document.location.href='?nocss';\" />";
}
?>