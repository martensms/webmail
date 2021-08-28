<?php
function uw_readable_size($number, $precision){
 $tags = array('B', 'KB', 'MB', 'GB', 'TB');
 $attag = 0;
 while($number>1024){
  $number = $number / 1024;
  $attag++;
 }
 return round($number, $precision).$tags[$attag];
}

function bc_array_search($needle,$haystack){
 if(in_array($needle,$haystack)){
  foreach($haystack as $key => $value){
   if($needle == $value){
    return $key;
   }
  }
 }else{
  return false;
 }
}

function get_offset() {
  global $cfg;
  $os = date('Z') / 60;
  if($os > 0 && $cfg{'west_of_utc'}) {
    $os = 0 - $os;
  }
  if($os < 0) { 
    $minus = "-";
    $os = 0 - $os;
  } else {
    $minus = "+";
  }
  $hours = intval($os / 60);
  $minutes = $os - ($hours * 60);
  return sprintf("%s%02d%02d", $minus, $hours, $minutes);
}

function get_offset_minutes() {
  global $cfg;
  $os = date('Z') / 60;
  if($os > 0 && $cfg{'west_of_utc'}) {
    $os = 0 - $os;
  }
  return $os;
}

function get_real_username($acc=false,$cfg){
 if(is_array($cfg['usermap'])){
  foreach($cfg['usermap'] as $id=>$name){
   if($id==$acc){ return $name; }
  }
 }
 return $acc;
}

function get_remotefile($url) {
    if($url == false || empty($url)) return false;
    $options = array(
    	CURLOPT_URL            => $url,     // URL of the page
    	CURLOPT_RETURNTRANSFER => true,     // return web page
    	CURLOPT_HEADER         => false,    // don't return headers
    	CURLOPT_ENCODING       => "",       // handle all encodings
    	CURLOPT_USERAGENT      => "webmail",
    	CURLOPT_AUTOREFERER    => true,
    	CURLOPT_CONNECTTIMEOUT => 120,
    	CURLOPT_TIMEOUT        => 120,
    	CURLOPT_MAXREDIRS      => 3,        // stop after 3 redirects
    );

    $ch      = curl_init($url);
    curl_setopt_array($ch,$options);
    $content = curl_exec( $ch );
    $header  = curl_getinfo( $ch );
    curl_close( $ch );
    return $content;
}
?>
