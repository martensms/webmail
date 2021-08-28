<?php
class uw_message_show{
 var $cid_list;
 var $folder;
 var $msgnum;
 var $mailbox;
 var $messages_shown = 0;
 var $config;
 var $cache;
 var $cache2;

 function __construct(&$config=false){
  if($config){
   $this->config = $config;
  }
 }

 function display_message($mailbox,$folder,$msgnum,$struct,$imap_part_base="",$isrfc822=""){
  $this->mailbox = $mailbox;
  $this->folder = $folder;
  $this->msgnum = $msgnum;
  $this->isrfc822 = $isrfc822;
  $this->cid_list = $this->make_cid_list($struct->parts,$imap_part_base);
  $this->process_multipart_mixed(array($struct),$imap_part_base,1);

  if(!preg_match("~(<br( {0,1})?\/?>)~Uis",$this->cache)){
   $this->cache=preg_replace("~\n~Uis","<br/>",$this->cache);
  }
  return $this->cache.$this->cache2;
 }

 function make_cid_list($struct_parts,$imap_part_base){
  $cid_list=array();
  for($i=0;$i<count($struct_parts);$i++){
   if($imap_part_base==""){
    $imap_part=($i+1);
   }else{
    $imap_part=$imap_part_base.".".($i+1);
   }
   $struct=$struct_parts[$i];
   if($struct->type==1){
    $get_cids=$this->make_cid_list($struct->parts,$imap_part);
    for($j=0;$j<count($get_cids);$j++){
     $cid_list[]=$get_cids[$j];
    }
   }else{
    if($struct->ifid==1){
     $id=ereg_replace("<|>","",$struct->id);
     $cid_list[]=array(
      "cid" => $id,
      "imap_part" => $imap_part,
      "struct" => $struct
     );
    }
   }
  }
  return $cid_list;
 }

 function convert_encoding($struct,$text){
  if($struct->encoding==3){
   $text = base64_decode($text);
  }elseif($struct->encoding==4){
   $text = $this->quoted_printable_decode($text);
  }
  return $text;
 }

 function process_attachment($struct,$imap_part){
  $types = array(
   0 => "TEXT",
   1 => "MULTIPART",
   2 => "MESSAGE",
   3 => "APPLICATION",
   4 => "AUDIO",
   5 => "IMAGE",
   6 => "VIDEO",
   7 => "OTHER"
  );
  if($struct->type){
   $type=$types[$struct->type];
  }else{
   $type="TEXT";
  }
  if($struct->subtype){
   $subtype=$struct->subtype;
  }else{
   $subtype="TEXT";
  }
  $mime_type = strtolower($type."/".$subtype);
  if($struct->ifdparameters == 1){
   for($i=0;$i<count($struct->dparameters);$i++){
    if(strtolower($struct->dparameters[$i]->attribute)=="filename"){
     $filename = $struct->dparameters[$i]->value;
    }
   }
  }
  if($filename==""){
   $filename="unknown";
  }
  if($struct->encoding==3){
   $dl="dl_b64";
  }elseif($struct->encoding==4){
   $dl="dl";
  }else{
   $dl="dl";
  }
  $size = $struct->bytes;
  $folder = $this->folder;
  $msgnum = $this->msgnum;
  $download = "?popup:".$dl.":".urlencode($filename).":$type/$subtype:$folder:$msgnum:$imap_part";
  $img = get_mime_pic($type,$subtype,$this->config);
  if(strlen($filename)>15){ $filename = substr($filename,0,15)."..."; }

  $this->cache2.= "
<div class=\"attach\">
 <img src=\"".$img."\" />
 <span><a href=\"$download\">".$filename."</a><br/>
 $mime_type<br/>$size Bytes</span>
</div>";
 }

 function process_text_html($struct,$imap_part){
  $mailbox = $this->mailbox;
  $msgnum = $this->msgnum;
  $folder = $this->folder;
  $text = imap_fetchbody($mailbox,$msgnum,$imap_part,FT_UID);
  $text = $this->convert_encoding($struct,$text);
  $text = preg_replace("/<(\s*)title(\s*)>(.*?)<(\s*)\/(\s*)title(\s*)>/is", "",$text);
  $text = preg_replace("/<(\/*)(\s*)(html|head|title|script|body|meta)(.*?)>/is", "",$text);
  $text = preg_replace("/\s+on\w+\s*\=/is", " onXXX=",$text);
  for($i=0;$i<count($this->cid_list);$i++){
   if(ereg($this->cid_list[$i]["cid"],$text)){
    $cid = $this->cid_list[$i]["cid"];
    $type = strtolower($this->cid_list[$i]['struct']->subtype);
    $imap_part = $this->cid_list[$i]['imap_part'];
    $cid_imgsrc = "?popup:dl::image/$type:$folder:$msgnum:$imap_part";
    $text = ereg_replace("cid:$cid",$cid_imgsrc,$text);
   }
  }
  if($this->messages_shown>0){
   $this->cache.= "<hr size=\"1\" />";
  }
  $this->cache.= "$text\n";
  $this->messages_shown++;
 }

 function process_text_plain($struct,$imap_part){
  $mailbox = $this->mailbox;
  $msgnum = $this->msgnum;
  if($imap_part==""){
   $text = imap_body($mailbox,$msgnum,FT_UID);
  }else{
   $text = imap_fetchbody($mailbox,$msgnum,$imap_part,FT_UID);
  }
  $text = $this->convert_encoding($struct, $text);
  $text = htmlentities($text);
  $text = preg_replace("~\n~Uis", "<br/>", $text);
  if($this->messages_shown>0){
   $this->cache.= "<hr />";
  }

  $text = preg_replace("'(\s+)http://(.*?)(\s+)'", "\\1<a href=\"http://\\2\" target=\"_blank\">http://\\2</a>\\3" , $text);
  $text = preg_replace("'(\s+)https://(.*?)(\s+)'", "\\1<a href=\"https://\\2\" target=\"_blank\">https://\\2</a>\\3" , $text);
  $text = preg_replace("'(\s+)www.(.*?)(\s+)'", "\\1<a href=\"http://www.\\2\" target=\"_blank\">www.\\2</a>\\3" , $text);
  $text = preg_replace("'([A-Za-z0-9_\-.]+)@([A-Za-z0-9_\-.]+)'","<a href=\"?compose:new:$1@$2\">$1@$2</a>" , $text);
  $this->cache.= "<tt>$text</tt>\n";
  $this->messages_shown++;
 }

 function process_multipart_related($struct_parts,$imap_part_base){
  for($i=0;$i<count($struct_parts);$i++){
   if($imap_part_base==""){
    $imap_part=($i+1);
   }else{
    $imap_part=$imap_part_base.".".($i+1);
   }
   $struct=$struct_parts[$i];
   if($struct->type==1){
    if($struct->subtype=="ALTERNATIVE"){
     $this->process_multipart_alternative($struct->parts,$imap_part);
    }
   }elseif($struct->type==0){
    if($struct->subtype=="HTML"){
     $this->process_text_html($struct,$imap_part);
    }
   }
  }
 }

 function process_multipart_alternative($struct_parts,$imap_part_base){
  $types = array(
   0 => "TEXT",
   1 => "MULTIPART",
   2 => "MESSAGE",
   3 => "APPLICATION",
   4 => "AUDIO",
   5 => "IMAGE",
   6 => "VIDEO",
   7 => "OTHER"
  );
  $types_array = array();
  for($i=0;$i<count($struct_parts);$i++){
   $struct = $struct_parts[$i];
   $types_array[$struct->subtype] = $i;
  }
  if(isset($types_array["RELATED"])){
   $i = $types_array["RELATED"];
   $struct = $struct_parts[$i];
   if($imap_part_base==""){
    $imap_part = ($i + 1);
   }else{
    $imap_part = $imap_part_base.".".($i+1);
   }
   $this->process_multipart_related($struct->parts,$imap_part);
  }elseif(isset($types_array["HTML"])){
   $i = $types_array["HTML"];
   $struct = $struct_parts[$i];
   if($imap_part_base==""){
    $imap_part = ($i+1);
   }else{
    $imap_part = $imap_part_base.".".($i+1);
   }
   $this->process_text_html($struct,$imap_part);
  }elseif(isset($types_array["PLAIN"])){
   $i = $types_array["PLAIN"];
   $struct = $struct_parts[$i];
   if($imap_part_base==""){
    $imap_part = ($i + 1);
   }else{
    $imap_part = $imap_part_base.".".($i+1);
   }
   $this->process_text_plain($struct,$imap_part);
  }
 }

 function process_multipart_mixed($struct_parts,$imap_part_base,$isbase=0){
  for($i=0;$i<count($struct_parts);$i++){
   if($imap_part_base==""){
    $imap_part=($i+1);
   }else{
    $imap_part=$imap_part_base.".".($i+1);
   }
   $struct=$struct_parts[$i];

   if($struct->type==1){
    $imap_part=($isbase?$imap_part_base:$imap_part);
    if($struct->subtype=="MIXED"){
     $this->process_multipart_mixed($struct->parts,$imap_part);
    }elseif($struct->subtype=="PARALLEL"){
     $this->process_multipart_mixed($struct->parts,$imap_part);
    }elseif($struct->subtype=="APPLEDOUBLE"){
     $this->process_multipart_mixed($struct->parts,$imap_part);
    }elseif($struct->subtype=="RELATED"){
     $this->process_multipart_related($struct->parts,$imap_part);
    }elseif($struct->subtype=="ALTERNATIVE"){
     $this->process_multipart_alternative($struct->parts,$imap_part);
    }
   }elseif($struct->type==0){
    if(strtoupper($struct->disposition)=="ATTACHMENT"){
     $this->process_attachment($struct,$imap_part);
    }else{
     if($struct->subtype=="HTML"){
      $this->process_text_html($struct,$imap_part);
     }elseif($struct->subtype=="PLAIN"){
      $this->process_text_plain($struct,$imap_part);
     }else{
      $this->process_attachment($struct,$imap_part);
     }
    }
   }elseif($struct->type==2){
    if($struct->subtype=="DELIVERY-STATUS"){
     $this->process_text_plain($struct,$imap_part);
    }elseif($struct->subtype=="RFC822"){
     $struct   = $struct->parts[0];
     $msg      = imap_rfc822_parse_headers(imap_fetchbody($this->mailbox,$this->msgnum,"$imap_part.0",FT_UID));
     $msginfo = array(
      'subject' => ($msg->subject?$msg->subject:'[No Subject]'),
      'from_name' => ($msg->from[0]->personal?$msg->from[0]->personal:$msg->from[0]->mailbox.'@'.$msg->from[0]->host),
      'from_address' => ($msg->reply_to[0]->mailbox?$msg->reply_to[0]->mailbox.'@'.$msg->reply_to[0]->host:$msg->from[0]->mailbox.'@'.$msg->from[0]->host)
     );
     $filename=urlencode(time().".eml");
     if(strlen($filename)>15){ $filename = substr($filename,0,15)."..."; }
     $img = get_mime_pic("message","rfc822",$this->config);
     $this->cache2.= "
<div class=\"attach\">
<img src=\"".$img."\" />
<span title=\"".$msginfo['subject']."\">
<a href=\""."?popup:dl:".$filename.":message/rfc822:".$this->folder.":".$this->msgnum."\">".$filename."</a><br/>
(from <a href=\"?compose:new:".$msginfo['from_address']."\">".$msginfo['from_name']."</a>)</span>
<br/><br/>
</div>";

     $message_show = new uw_message_show();
     $message_show->display_message($this->mailbox,$this->folder,$this->msgnum,$struct,$imap_part);
    }
   }else{
    $this->process_attachment($struct,$imap_part);
   }
  }
 }

 function find_first_text($mailbox,$msgnum,$struct,$imap_part_base=""){
  $text = $this->find_first_text_recursive($mailbox,$msgnum,$struct->parts,$imap_part_base);
  if($text){
   return $text;
  }else{
   if($struct->subtype=="PLAIN"){
    $text = imap_body($mailbox, $msgnum, FT_UID);
    if($struct->encoding==3){
     $text = base64_decode($text);
    }elseif($struct->encoding==4){
     $text = $this->quoted_printable_decode($text);
    }
    return $text;
   }
  }
 }
      
 function find_first_text_recursive($mailbox,$msgnum,$struct_parts,$imap_part_base=""){
  for($i=0;$i<count($struct_parts);$i++){
   if($imap_part_base==""){
    $imap_part = ($i + 1);
   }else{
    $imap_part = $imap_part_base.".".($i + 1);
   }
   $struct = $struct_parts[$i];
   if($struct->type==1){
    $found_text = $this->find_first_text_recursive($mailbox,$msgnum,$struct->parts,$imap_part);
    if($found_text){
     return $found_text;
    }
   }else{
    if($struct->subtype=="PLAIN"){
     $text = imap_fetchbody($mailbox, $msgnum, $imap_part, FT_UID);
     if($struct->encoding==3){
      $text = base64_decode($text);
     }elseif($struct->encoding==4){
      $text = $this->quoted_printable_decode($text);  
     }
     return $text;
    } 
   }
  }
 }

 function get_message_forward($mailbox,$action,$folder,$msgnum,&$to,&$subject,&$body){
  $msg = imap_header($mailbox,imap_msgno($mailbox,$msgnum));
  $subject = !$msg->Subject?"[No Subject]":$msg->Subject;
  if($action=="reply"){
   if(!preg_match("/^re:/i", $subject)){
    $subject = "Re: $subject";
   }
  }elseif($action=="forward"){
   $subject = "Fwd: $subject";
  }
  if($action=="reply"){
   if($msg->reply_toaddress){
    $to = htmlentities($msg->reply_toaddress);
   }elseif($msg->fromaddress){
    $to = $msg->fromaddress;
   }elseif($msg->senderaddress){
    $to = $msg->fromaddress;
   }
  }

  $struct = imap_fetchstructure($mailbox,$msgnum,FT_UID);
  $body = $this->find_first_text($mailbox,$msgnum,$struct,"");
  if($body!=""){
   $body = preg_replace("/^/m", ">", $body);
   $fromaddress = $to;
   $body = "\n\n\nOn $msg->date, $fromaddress said:\n".$body;
  }
 }

 function quoted_printable_decode($text){
  $text = preg_replace("/=(\r*)\n/", "", $text);
  $text = preg_replace("/=([A-Fa-f0-9]{2})/e", "pack('H2', '\\1')", $text);
  return $text;
 }
}
?>