<?php
#?popup:dl:filename.eml:MESSAGE/RFC822:INBOX:3
#?popup:dl_b64:filename.jpg:IMAGE/JPEG:INBOX:7:2
#?popup:headers:::INBOX:3

#    0   1  2             3          4   5 6

$folder = urldecode($query[4]?$query[4]:$cfg['imap_mainbox']);
$obj['imap']->connect(urldecode($folder));

if(preg_match("~dl~Uis",$query[1])){
 header("Content-type: ".$query[3]);
 header("Content-Disposition: attachment; filename=\"".urldecode($query[2])."\"");

 if($query[6]){
  $body = imap_fetchbody($obj['imap']->mbox, $query[5], $query[6], FT_UID);
  if(preg_match("~(Message/RFC822)~Uis",$query[3])){
   $body= imap_fetchbody($obj['imap']->mbox, $query[5], "$part_no.0", FT_UID).$body;
  }
 }else{
  $body = imap_fetchheader($obj['imap']->mbox, $query[5], FT_UID + FT_PREFETCHTEXT);
  $body.= imap_body($obj['imap']->mbox, $query[5], FT_UID);
 }
 if(preg_match("~b64~Uis",$query[1])){
  echo imap_base64($body); exit;
 }else{
  echo $body; exit;
 }
}else{
 $msginfo = $obj['imap']->retrieve_message_info($query[5]);
 $cfg['tpl']['cnt']['headers']=nl2br(htmlentities($obj['imap']->retrieve_message_headers_text($folder, $query[5])));

 $cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
<a href=\"?config\">Index</a> |
<a href=\"?mailbox:".urlencode($folder).":".$query[2]."\">Mailbox</a> |
<a href=\"?folders\">Folders</a> |
<a href=\"?uspace:".urlencode($folder)."\">uSpace<sup>New!</sup></a> |
<a href=\"?compose\">Compose</a> |
<br/>
| <a href=\"?message:".urlencode($folder).":".$query[5]."\">Message</a> |
</div>";
}
?>
