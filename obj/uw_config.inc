<?php
$cfg=array(
  // If this is a POP3 connection, 0 is assumed.
  'display_folders' => 1,
  // This options controls whether copies of sent messages should be
  // deposited in 'imap_sentbox' (see below).
  // If 'display_folders' is 0, 0 is assumed here as well.
  'use_sentbox' => 0,
  // Path relative to 'imap_mailpath' where copies of sent mail go.
  'imap_sentbox' => 'sent-mail',
  // How many messages should be displayed in the mailbox per page?
  'mailbox_page_size' => 20,

  // Set to 1 if you want sendmail virtual user table to username mapping.
  // If you have a virtusetable entry "joe@domain.com juser" and 'host'
  // above is "domain.com" and the end user types in "joe" and a username
  // and the correct password for "juser", uwmail will log in as "juser".
  // Email sent will appear from "joe@domain.com" instead of
  // "juser@domain.com".  This is extremely useful if you run an ISP with
  // multiple mail domains handled.  This option requires PHP to be compiled
  // with "--with-db".
  'use_virtusertable' => 0,

  // In the example below, the path/filename is
  // really '/etc/mail/virtusertable.db'; PHP will add the extension.
  'virtusertable_file' => '/etc/mail/virtusertable',

  // Is your local time zone west of UTC?  This flag is not used if you
  // have the latest version of PHP (IE, it's figured out automatically),
  // but it doesn't hurt to set it anyways.
  'west_of_utc' => 1,
  'imap_timezone_offset' => 0
);

if($_COOKIE['advcfg']=="simple" || !isset($_COOKIE['advcfg'])){
 foreach($pfs[0] as $name=>$value){
  $cfg[$name]=$value;
 }
}elseif($_COOKIE['advcfg']=="manual"){
 foreach($_COOKIE as $name=>$value){
  if($name!="username" && $name!="password"){
   $cfg[$name]=$value;
  }
 }
}elseif($_COOKIE['advcfg']=="xmlfile"){
# well, todo...


 if($cache=get_remotefile($_COOKIE['url'])){
  if($cache=preg_replace("~(({uwmail_profile})(.*)({/uwmail_profile}))~Uis","$3",$cache)){
   $cache=preg_replace("~\n~Uis","",$cache);
   $nodes=explode(";",$cache);
   foreach($nodes as $id=>$value){
    $subnode=explode(":",$value);
    if($subnode[0]!=""){
     $cfg[$subnode[0]] = $subnode[1];
    }
   }
  }else{
   $_COOKIE['error']=$_COOKIE['url']." is not a uwmail profile.";
  }
 }else{
  $_COOKIE['error']=$_COOKIE['url']." couldn't be opened.";
 }

}elseif(preg_match("~^profile([0-9]{1,2})$~Uis",$_COOKIE['advcfg'])){
 $id=preg_replace("~(profile)~Uis","",$_COOKIE['advcfg']);
 foreach($pfs[$id] as $name=>$value){
  $cfg[$name]=$value;
 }
}

$cfg['is_pop3'] = (preg_match("/\/pop3$/",$cfg['imap_host'])?1:0);
if($cfg['is_pop3'] && ($cfg['display_folders'])) {
  $cfg['display_folders'] = 0;
}
?>
