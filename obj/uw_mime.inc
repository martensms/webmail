<?php
if(is_dir($dir=$cfg['tpl']['css']['url']."/mime")){
 $handle=opendir($dir);
 while (false!==($file=readdir($handle))){
  if(is_file($dir."/".$file)){
   $string=preg_replace("~\.png~Uis","",$file);
   if(!preg_match("~_~Uis",$string)){
    $cfg['mime'][$string]=true;
   }else{
    $string=explode("_",$string);
    $cfg['mime']['_sub'][$string[0]][$string[1]] = true;
   }
  }
 }
}

function get_mime_pic($type=false,$subtype=false,$cfg=false){
 $type=strtolower($type);
 $subtype=strtolower($subtype);
 if(isset($cfg['mime']['_sub'][$type])){
  foreach($cfg['mime']['_sub'][$type] as $msubtype=>$dummy){
   if(preg_match("~".$msubtype."~Uis",$subtype)){
    return $cfg['tpl']['css']['url']."/mime/".$type."_".$msubtype.".png";
   }
  }
 }
 if($cfg['mime'][$type]){
  return $cfg['tpl']['css']['url']."/mime/".$type.".png";
 }
 return $cfg['tpl']['css']['url']."/mime/unknown.png";
}
?>