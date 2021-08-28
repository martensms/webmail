<?php
#?folders:INBOX:delete
#?folders::create -> input[folder]

$folder = urldecode($query[1]?$query[1]:$cfg['imap_mainbox']);
$obj['imap']->connect($folder);

$cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
<a href=\"?config\">Index</a> |
<a href=\"?mailbox:".urlencode($folder)."\">Mailbox</a> |
<a href=\"?folders\">Folders</a> |
<a href=\"?uspace:".urlencode($folder)."\">uSpace</a><sup>New!</sup> |
<a href=\"?compose\">Compose</a> |
</div>";

if($query[2]=="delete"){
 $input['folder']=$query[1];
}
switch($query[2]){
 case "delete":
  if($input['folder']!="INBOX"){
   $obj['imap']->delete_mailbox($folder);
  }
  header("Location: ?folders");
 break;
 case "create":
  if($input['folder']!="INBOX") {
   $obj['imap']->create_mailbox($input['folder']);
  }
  header("Location: ?folders");
 break;
 case "rename":
  if($input['old']!="INBOX" && $input['new']!="INBOX" && $input['old']!="- select folder -"){
   $obj['imap']->rename_mailbox($input['old'],$input['new']);
  }
  header("Location: ?folders");
 break;
}

$boxes = $obj['imap']->retrieve_mailboxes();
$total['msgs']   = 0;
$total['unread'] = 0;
$total['size']   = 0;

$obj['imap']->disconnect();

$cfg['tpl']['cnt']['table']="
 <table id=\"folders\">
  <tr> 
   <th>(Select)</th>
   <th>Mailbox</th>
   <th>Messages</th>
   <th>New</th>
   <th>Size</th>
   <th>(Delete)</th>
  </tr>";

$inbox_shown=false;
for($i=0;$i<count($boxes);$i++){
 $total['msgs'] += $boxes[$i]['msgs'];
 $total['unread'] += $boxes[$i]['unread'];
 $total['size'] += $boxes[$i]['size'];
 if($boxes[$i]['name']!="INBOX" || ($boxes[$i]['name']=="INBOX" && !$inbox_shown)){
  $cfg['tpl']['cnt']['table'].="
  <tr> 
   <td>";
  if($folder==$boxes[$i]['name']){
   $cfg['tpl']['cnt']['table'].="<img title=\"selected folder\" src=\"./tpl/light_blue/quota/selected.png\"/>";
  }else{
   $cfg['tpl']['cnt']['table'].="<a title=\"select folder\" href=\"?folders:".urlencode($boxes[$i]['name'])."\"><img class=\"load\" src=\"./tpl/light_blue/quota/select.png\" alt=\"select\"/></a>";
  }
  $cfg['tpl']['cnt']['table'].="</td>
   <td><a href=\"?mailbox:".urlencode($boxes[$i]['name'])."\">".htmlentities($boxes[$i]['name'])."</a></td>
   <td>".$boxes[$i]['msgs']."</td>
   <td>".$boxes[$i]['unread']."</td>
   <td>".uw_readable_size($boxes[$i]['size'],1)."</td>
   <td>";
  if($boxes[$i]['name']!="INBOX"){
   $cfg['tpl']['cnt']['table'].="<a href=\"?folders:".urlencode($boxes[$i][name]).":delete\"><img src=\"".$cfg['tpl']['css']['url']."/quota/delete.png\" alt=\"delete\"/></a>";
  }else{
   $cfg['tpl']['cnt']['table'].="&nbsp;";
  }
  $cfg['tpl']['cnt']['table'].="
   </td>
  </tr>";
 }
 if($boxes[$i]['name']=="INBOX"){
  $inbox_shown=true;
 }
}

$cfg['tpl']['cnt']['table'].="
  <tr> 
   <td>&nbsp;</td>
   <th>Total</th>
   <td>".$total['msgs']."</td>
   <td>".$total['unread']."</td>
   <td>".uw_readable_size($total['size'],1)."</td>
   <td>&nbsp;</td>
  </tr>
 </table>";

for($i=0;$i<count($boxes);$i++){
 if($boxes[$i]['name']!="INBOX"){
  $cfg['tpl']['cnt']['rename'].="
    <option value=\"".htmlentities($boxes[$i]['name'])."\">".htmlentities($boxes[$i]['name'])."</option>";
 }
}
?>