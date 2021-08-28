<?php
$folder = urldecode($query[1]?$query[1]:$cfg['imap_mainbox']);
$obj['imap']->connect(urldecode($folder));

$cfg['tpl']['var']['folder'] = urlencode($folder);
$cfg['tpl']['var']['sentbox']= (stristr($folder,$cfg['imap_sentbox'])?'To':'From');

if($input['delete']){
 if(count($input['select'])>0){
  $obj['imap']->delete_messages(array_keys($input['select']));
 }
}elseif($input['undelete']){
 if(count($input['select'])>0){
  $obj['imap']->undelete_messages(array_keys($input['select']));
 }
}elseif($input['move']){
 $obj['imap']->move_messages($folder,array_keys($input['select']), "uSpace");
}elseif($input['expunge']){
 $obj['imap']->expunge_messages();
}

$offset = ($query[2]?$query[2]:1);
$return = $cfg['mailbox_page_size'];
$count  = @$obj['imap']->retrieve_num_messages();
$msgs   = @$obj['imap']->retrieve_message_list($offset, $return);
$boxes  = @$obj['imap']->retrieve_mailboxes_short();
$obj['imap']->disconnect();

for($i=0;$i<count($msgs);$i++){
 if($msgs[$i]['deleted']){
  $bgclass = "deleted";
  $show_undelete=true;
 }elseif($cfg['is_pop3']){
  $bgclass = "read";
 }elseif($msgs[$i]['unread']){
  $bgclass = "unread";
 }else{
  $bgclass = "read";
 }
 $msg_date = date("Y/m/d", $msgs[$i]['udate']);
 if($msg_date==date("Y/m/d")){
  $msg_date=date("g:ia", $msgs[$i]['udate']);
 }

$cfg['tpl']['cnt']['table'].="
  <tr class=\"".$bgclass."\"> 
   <td class=\"checkbox\"><input type=\"checkbox\" name=\"input[select][".$msgs[$i]['msgno']."]\" /></td>
   <td><a href=\"?compose:reply:".urlencode($folder).":".$msgs[$i][msgno]."\">
    ".htmlentities((stristr($folder,$cfg['imap_sentbox'])?$msgs[$i]['to']:$msgs[$i]['from']))."</a></td>
   <td>".($msgs[$i]['count_mime']['message/rfc822']?'<img src="'.$cfg['tpl']['css']['url'].'/msg/text.png" alt="*" title="contains rfc822" />':'')
        .($msgs[$i]['count_disposition']['attachment']?'<img src="'.$cfg['tpl']['css']['url'].'/msg/file.png" alt="*" title="contains files" />':'')
        .($msgs[$i]['count_mime']['text/html']?'<img src="'.$cfg['tpl']['css']['url'].'/msg/html.png" alt="*" title="contains html" />':'')
        .($msgs[$i]['replied']?'<img src="'.$cfg['tpl']['css']['url'].'/msg/replied.png" alt="*" title="is already replied" />':'')
   ."&nbsp;<a href=\"?message:".urlencode($folder).":".$msgs[$i]['msgno']."\">".htmlentities($msgs[$i]['subject'])."</a>
   </td>
   <td>".uw_readable_size($msgs[$i]['size'], 1)."</td>
   <td>".$msg_date."</td>
  </tr>";
}

if($count==0){
 $cfg['tpl']['cnt']['table'].="
  <tr class=\"read\"> 
   <td colspan=\"5\" align=\"center\"><b>This folder is empty.</b></td>
  </tr>";
}else{
 $cfg['tpl']['cnt']['table'].="
  <tr class=\"infobar\">
   <td colspan=\"5\" align=\"center\">
    Messages ".$offset."-".(($offset+$return)>$count?$count:($offset+$return-1))." of ".$count." in ".htmlentities($folder)."<br/> |
    <input type=\"submit\" name=\"input[delete]\" value=\"Delete\" /> |";
 if($show_undelete){ $cfg['tpl']['cnt']['table'].="
    <input type=\"submit\" name=\"input[undelete]\" value=\"Undelete\" /> |"; }
 $cfg['tpl']['cnt']['table'].="
    <input type=\"submit\" name=\"input[expunge]\" value=\"Expunge\" /> |
    <input type=\"submit\" name=\"input[move]\" value=\"Move\" /> (to uSpace) |
   </td>
  </tr>";
}

$cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
";

if($offset > 1){
 $newoffset=(($offset - $return)<1?0:($offset - $return));
 $cfg['tpl']['cnt']['toolbar'].=" <a href=\"?mailbox:".urlencode($folder).":".$newoffset."\">&lt;&lt;</a> |";
}else{
 $cfg['tpl']['cnt']['toolbar'].=" <a href=\"#\">&lt;&lt;</a> |";
}

$cfg['tpl']['cnt']['toolbar'].="
<a href=\"?config\">Index</a> |
<a href=\"?mailbox:".urlencode($folder)."\">Mailbox</a> |
<a href=\"?folders:".urlencode($folder)."\">Folders</a> |
<a href=\"?uspace:".urlencode($folder)."\">uSpace</a><sup>New!</sup> |
<a href=\"?compose\">Compose</a> |
";

if(($offset + $return)<=$count){
 $newoffset=$offset + $return;
 $cfg['tpl']['cnt']['toolbar'].="<a href=\"?mailbox:".urlencode($folder).":".$newoffset."\">&gt;&gt;</a> |";
}else{
 $cfg['tpl']['cnt']['toolbar'].="<a href=\"#\">&gt;&gt;</a> |";
}
 $cfg['tpl']['cnt']['toolbar'].="
</div>";
?>