<?php
# * CAPABILITY IMAP4 IMAP4rev1 LITERAL+ UIDPLUS NAMESPACE UNSELECT X-NETSCAPE STARTTLS
# 0 OK CAPABILITY Completed
# ?uSpace:directoryID
# ?uSpace::iframe && $input[folder]

if($query[1]!="iframe"){
 $folder = urldecode($query[1]?$query[1]:$cfg['imap_mainbox']);
}else{
 $folder = $cfg['imap_mainbox'];
}
$obj['imap']->connect($folder);

if($input['delete']){
 if(count($input['select'])>0){
  $obj['imap']->delete_messages(array_keys($input['select']));
  $obj['imap']->expunge_messages();
 }
}

switch($query[2]){
 default:
  $cfg['tpl']['file'] = "uspace.xhtml";
  $cfg['tpl']['var']['parentimg']  = $cfg['tpl']['css']['url']."/quota/parent.png";
  $cfg['tpl']['var']['refreshimg'] = $cfg['tpl']['css']['url']."/quota/refresh.png";
  $cfg['tpl']['var']['folder']     = $folder;
  $cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
<a href=\"?config\">Index</a> |
<a href=\"?mailbox\">Mailbox</a> |
<a href=\"?folders\">Folders</a> |
<a href=\"?uspace\">uSpace</a><sup>New!</sup> |
<a href=\"?compose\">Compose</a> |</div>";

  $temp = @imap_get_quotaroot($obj['imap']->mbox, "INBOX");
  if(is_array($temp)){
   $quota['sto'] = $temp['STORAGE'];
   $quota['sto']['usage']= round(($quota['sto']['usage']/1024),0);
   $quota['sto']['limit']= round(($quota['sto']['limit']/1024),0);

   $temp['perpx']=round((($quota['sto']['usage']/$quota['sto']['limit'])*100),2)*400/100;
   # $temp['perpx']-> 1,5% = 6px of 400px!

   $temp['csspx']=(400-$temp['perpx'])*(-1);

   $cfg['tpl']['cnt']['quota']="<div class=\"quota\">
 <span style=\"background-position: ".$temp['csspx']."px\">".$quota['sto']['usage']." MB / ".$quota['sto']['limit']." MB</span>
</div>";

   unset($temp);
  }else{
   $cfg['tpl']['cnt']['quota']="";
  }
  $count  = @$obj['imap']->retrieve_num_messages();
  $temp   = @$obj['imap']->retrieve_message_list(1, 20);

  for($i=0;$i<count($temp);$i++){
   if(preg_match("~(uSpace\|)(.*)~Uis",$temp[$i]['subject'])){
    $files[$i]=explode("|",$temp[$i]['subject']);
    $cfg['tpl']['cnt']['files'].="
   <tr>
    <td><input type=\"checkbox\" name=\"input[select][".$temp[$i]['msgno']."]\"/></td>
    <td><a href=\"?popup:dl_b64:".urlencode($files[$i][1])."::".urlencode($folder).":".$temp[$i]['msgno'].":2\">".$files[$i][1]."</a></td>
   </tr>";
   }
  }
  if(!$cfg['tpl']['cnt']['files']){
   $cfg['tpl']['cnt']['files']="
   <tr><td colspan=\"5\" align=\"center\"><b>This folder is empty.</b></td></tr>";
  }
 break;
 case "upload":
  $cfg['tpl']['file']         = "_blank.xhtml";
  $cfg['cnt']['toolbar']      = "";
  $cfg['tpl']['cnt']['error'] = "";

  $texts_array = array();
  $texts_array[] = array(
   'content_type' => "text/html",
   'message' => "Content uploaded via uSpace");

  if($_FILES['attach']['name']!=""){
   # - - - construct the message - - - #
   $obj['mail']       = new uw_smtp();
   $obj['mail']->imap = $obj['imap'];
   $obj['mail']->recipient = get_real_username($_COOKIE['username'],&$cfg).'@'.$cfg['real_host'];
   $obj['mail']->sender    = get_real_username($_COOKIE['username'],&$cfg).'@'.$cfg['real_host'];
   $obj['mail']->subject   = "uSpace|".$_FILES['attach']['name'];
   $obj['mail']->texts     = $texts_array;
   $obj['mail']->attachments[] = $_FILES['attach'];
   $obj['mail']->build_message();

   if($obj['mail']->send()){
    $cfg['tpl']['cnt']['body'] .= "<script type=\"text/javascript\">
top.document.getElementById('status".$input['id']."').innerHTML='\<img class=\"load\" src=\"".$cfg['tpl']['css']['url']."/quota/success.png\"\/>';
top.document.getElementById('file".$input['id']."').innerHTML=\"".$_FILES['attach']['name']."\";
</script>";
   }else{
    $cfg['tpl']['cnt']['body'] .= "<script type=\"text/javascript\">
top.document.getElementById('status".$input['id']."').innerHTML='\<img class=\"load\" src=\"".$cfg['tpl']['css']['url']."/quota/fail.png\"\/>';
top.document.getElementById('file".$input['id']."').innerHTML=\"".$_FILES['attach']['name']."\";
</script>";
   }
   $obj['imap']->disconnect();
  }else{
   $cfg['tpl']['cnt']['body'] .= "<script type=\"text/javascript\">
top.document.getElementById('status".$input['id']."').innerHTML='\<img class=\"load\" src=\"".$cfg['tpl']['css']['url']."/quota/fail.png\"\/>';
top.document.getElementById('file".$input['id']."').innerHTML=\"Upload failed.\";
</script>";
  }
 break;
}

?>