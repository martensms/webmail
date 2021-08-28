<?php
$folder = urldecode($query[1]?$query[1]:$cfg['imap_mainbox']);
$obj['imap']->connect($folder);
$msginfo = $obj['imap']->retrieve_message_info($query[2]);

if($query[0]=="message_iframe"){
 $cfg['tpl']['file']         = "_blank.xhtml";
 $cfg['tpl']['cnt']['error'] = "";
 $struct = imap_fetchstructure($obj['imap']->mbox, $query[2], FT_UID);
 $obj['msg_show'] = @new uw_message_show($cfg);

 $cache=$obj['msg_show']->display_message($obj['imap']->mbox, $folder, $query[2], $struct);
 $cache=stripslashes($cache);

 $cfg['tpl']['cnt']['body'] = $cache;

}else{
 $cfg['tpl']['cnt']['toolbar']="
<div class=\"toolbar\"> |
<a href=\"?config\">Index</a> |
<a href=\"?mailbox:".urlencode($folder)."\">Mailbox</a> |
<a href=\"?folders\">Folders</a> |
<a href=\"?uspace:".urlencode($folder)."\">uSpace</a><sup>New!</sup> |
<a href=\"?compose\">Compose</a> |
</div>";

 $cfg['tpl']['var']['compose_link']="<a href=\"?compose:new:".$msginfo['from_address']."\">".$msginfo['from_name']." (".$msginfo['from_address'].")</a>";

 for($i=0;$i<count($msginfo['to_array']);$i++){
 $cfg['tpl']['cnt']['to_list'].="
 <a href=\"?compose:new:".$msginfo['to_array'][$i]['address']."\">".$msginfo['to_array'][$i]['name']." (".$msginfo['to_array'][$i]['address'].")</a><br />";
 }
 if(count($msginfo['cc_array'])>0){
  for($i=0;$i<count($msginfo['cc_array']);$i++){
  $cfg['tpl']['cnt']['cc_list'].="
 <a href=\"?compose:new:".$msginfo['cc_array'][$i]['address']."\">".$msginfo['cc_array'][$i]['name']." (".$msginfo['cc_array'][$i]['address'].")</a><br />";
  }
 }else{ $cfg['tpl']['cnt']['cc_list']="&nbsp;"; }

 $cfg['tpl']['var']['subject']=$msginfo['subject'];
 $cfg['tpl']['var']['date']=htmlentities($msginfo[date]);
 $cfg['tpl']['cnt']['info']="
 &lt;<a href=\"?popup:headers:::".$folder.":".$query[2]."\">Show Headers</a>&gt;
 &lt;<a href=\"?popup:dl:".date('Ymd-His', strtotime($msginfo[date])).".eml::".$folder.":".$query[2]."\">Download Message</a>&gt;";
 $cfg['tpl']['var']['iframe_url']="?message_iframe:".$folder.":".$query[2];
}
?>