<?php
#?compose:reply:INBOX:1
#?compose:new:email@server.tld
#  0       1    2     3

$folder = urldecode($query[2]?$query[2]:$cfg['imap_mainbox']);
$cfg['tpl']['var']['folder']=$folder;
$cfg['tpl']['var']['fwd_msg'] = "disabled=\"disabled\" ";

if($query[1]=="new"){
 $input['to'] = ($query[2]?$query[2]:"");
}elseif($query[1]=="reply"){
 if(!is_numeric($query[3])){
  $cfg['tpl']['var']['fwd_msg']= "disabled=\"disabled\" ";
  $cfg['tpl']['var']['msgno']  = "";
 }elseif(is_numeric($query[3])){
  $cfg['tpl']['var']['fwd_msg']= "";
  $cfg['tpl']['var']['msgno']  = ($input['msgno']?$input['msgno']:$query[3]);

  $obj['imap']->connect($folder);
  $struct = imap_fetchstructure($obj['imap']->mbox, $query[3], FT_UID);
  $obj['msg_show'] = new uw_message_show();
  $obj['msg_show']->get_message_forward($obj['imap']->mbox,$query[1],$query[2],$query[3],$input['to'],$input['subject'],$input['body']);
  $obj['imap']->disconnect();
 }
}elseif($query[1]=="send"){
 $texts_array = array();
 $texts_array[] = array(
  'content_type' => ($input['send_html']?'text/html':'text/plain'),
  'message' => $input['body']);

 $rfc822_array = array();
 if($input['send_rfc822']){
  $rfc822_array[] = array(
   'folder' => $query[2],
   'msgnum' => $query[3]);
 }

 # - - - construct the message - - - #
 $obj['imap']->connect($folder);
 $obj['mail']       = new uw_smtp();
 $obj['mail']->imap = $obj['imap'];

 $obj['mail']->recipient = $input['to'];
 $obj['mail']->sender    = ($input['fake']?$input['from']:(get_real_username($_COOKIE['username'],&$cfg).'@'.$cfg['real_host']));
 $obj['mail']->cc        = $input['cc'];
 $obj['mail']->subject   = $input['subject'];
 $obj['mail']->texts     = $texts_array;
 if($_FILES['attach1']['name'] != ""){ $obj['mail']->attachments[]   = $_FILES['attach1']; }
 if($_FILES['attach2']['name'] != ""){ $obj['mail']->attachments[]   = $_FILES['attach2']; }
 if($_FILES['attach3']['name'] != ""){ $obj['mail']->attachments[]   = $_FILES['attach3']; }
 if($_FILES['attach4']['name'] != ""){ $obj['mail']->attachments[]   = $_FILES['attach4']; }
 $obj['mail']->rfc822_messages = $rfc822_array;

 if(is_numeric($query[3])){
  $obj['mail']->reply_referrer = array(
   'folder' => $folder,
   'msgnum' => $query[3]);
 }

 # - - - finally build the message - - - #
 $obj['mail']->build_message();

 $cfg['tpl']['var']['title'] = "Send Message";
 $cfg['tpl']['file']         = "_blank.xhtml";
 $cfg['tpl']['cnt']['body']  = "";
 if($obj['mail']->send()){
  $cfg['tpl']['cnt']['error']= "Mail was accepted for delivery.<br /><br /><a href=\"?mailbox\">Well, thanks - now let's go back to Mailbox</a>";

  if($cfg['display_folders'] && $cfg['use_sentbox']){ $obj['mail']->add_sent_mail(); }
  if($query[3] && !$cfg['is_pop3']){ $obj['imap']->set_replied(array($query[3])); }
 }else{
  $cfg['tpl']['cnt']['error']= "Mail was NOT accepted for delivery.<br /><br /><a href=\"javascript:history.go(-1);\">You want to retry?</a>";
 }
 $obj['imap']->disconnect();
}

# real_username ... e.g. username=web113p7, host=123.123.123.1, real one is hellobuddy@realhost.tld

$cfg['tpl']['var']['from']    = ($input['fake']?$input['from']:(get_real_username($_COOKIE['username'],&$cfg).'@'.$cfg['real_host']));
$cfg['tpl']['var']['to']      = ($input['to']?$input['to']:"");
$cfg['tpl']['var']['subject'] = ($input['subject']?$input['subject']:"[No subject]");
$cfg['tpl']['var']['body']    = ($input['body']?$input['body']:"\n\n\n\n\n- - - sent with webmail - - -");

$cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
<a href=\"?config\">Index</a> |
<a href=\"?mailbox\">Mailbox</a> |
<a href=\"?folders\">Folders</a> |
<a href=\"?uspace\">uSpace</a><sup>New!</sup> |
<a href=\"?compose\">Compose</a> |
</div>";

?>
