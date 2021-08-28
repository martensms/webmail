<?php
class uw_imap {
  var $host;
  var $port;
  var $username;
  var $password;
  var $mbox;
  var $config;

  function uw_imap($username,$password,&$config) {
    $this->config   = $config;
    $this->username = $username;
    $this->password = $password;
  }

  function parts_count($msgno) {
    $struct = imap_fetchstructure($this->mbox, $msgno, FT_UID);
    return $this->parts_count_recursive(array($struct), array());
  }
  function parts_count_recursive($struct, $array) {
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
    for($i = 0; $i < count($struct); $i++) {
      $mime = strtolower($types[($struct[$i]->type ? $struct[$i]->type : 0)] . '/' . $struct[$i]->subtype);
      $array['mime'][$mime]++;
      if($struct[$i]->disposition) {
        $array['disposition'][strtolower($struct[$i]->disposition)]++;
      }
      if($struct[$i]->parts) {
        $array = $this->parts_count_recursive($struct[$i]->parts, $array);
      }
    }
    return $array;
  }

  function check_login(&$error) {
    if($this->connect("INBOX")) {
      $this->disconnect();
      return true;
    } else {
      $error = imap_last_error();
      return false;
    }
  }

  function create_imap_string($folder = '') {
    if($this->config['imap_flags']) {
      $flags=$this->config['imap_flags']; 
    }
    if($folder == '') {
      $string = "{" . $this->config['imap_host'] .":" . $this->config['imap_port'] .$flags."}";
    } elseif($folder == $this->config[imap_mainbox]) {
      $string = "{" . $this->config['imap_host'] .":" . $this->config['imap_port'] .$flags."}" . $folder;
    } else{
      $string = "{" . $this->config['imap_host'] .":" . $this->config['imap_port'] .$flags."}" . $this->config[imap_mailpath] . $folder;
    }
    $_COOKIE['error'].= $string."<br/>\n";
    return $string;
  }

  function create_imap_string_short($folder) {
    if($folder == $this->config[imap_mainbox]) {
      return $folder;
    } else {
      return $this->config[imap_mailpath] . $folder;
    }
  }

  function connect($folder) {
    $this->mbox = @imap_open($this->create_imap_string($folder), $this->username, $this->password);
    if($this->mbox) {
      return true;
    } else {
      return false;
    }
  }

  function disconnect() {
    imap_close($this->mbox);
    return true;
  }

  function retrieve_num_messages() {
    return imap_num_msg($this->mbox);
  }

  function append_mailbox($folder, $text) {
    return imap_append($this->mbox, $this->create_imap_string($folder), $text);
  }

  function retrieve_mailboxes_short() {
    $imap_string = $this->create_imap_string();
    $mailpath = $this->config[imap_mailpath];
    $s_imap_string = str_replace("/", "\/", $imap_string);
    $s_mailpath = str_replace("/", "\/", $mailpath);
    $list = imap_listmailbox($this->mbox, $imap_string.$this->config[imap_mailpath], '*');
    $list[] = $imap_string . "INBOX";
    sort($list);
    $out = array();
    for($i = 0; $i < count($list); $i++) {
      $name = preg_replace("/^$s_imap_string(($s_mailpath)?)/", "", $list[$i]);
      $out[] = $name;
    }
    return $out;
  }

 function retrieve_mailboxes() {
  $list = $this->retrieve_mailboxes_short();
  $out = array();
  for($i = 0; $i < count($list); $i++){
   $gmailfix=preg_replace("~\[~Uis","\[",$this->config['imap_mailpath']);
   $gmailfix=preg_replace("~\]~Uis","\]",$gmailfix); 
   if(preg_match("~".$gmailfix."~Uis",$list[$i])){
    #echo $list[$i]."=".$this->create_imap_string($list[$i])."<br/>";
    $list[$i]=preg_replace("~".$gmailfix."~Uis","",$list[$i]);
   }
   imap_reopen($this->mbox, $this->create_imap_string($list[$i]));
   $info = imap_mailboxmsginfo($this->mbox);
   $out[$i] = array(
    'name' => $list[$i],
    'unread' => $info->Unread,
    'size' => $info->Size,
    'msgs' => $info->Nmsgs
   );
  }
  return $out;
 }

  function retrieve_message_info($msgnum) {
    global $config;

    $msg = imap_header($this->mbox, imap_msgno($this->mbox, $msgnum));
    $to_array = array();
    for($i = 0; $i < count($msg->to); $i++) {
      $to_array[$i][name] = ($msg->to[$i]->personal ? $this->decode_header_string($msg->to[$i]->personal) : $msg->to[$i]->mailbox . '@' . $msg->to[$i]->host);
      $to_array[$i][address] = $msg->to[$i]->mailbox . '@' . $msg->to[$i]->host;
    }
    $cc_array = array();
    for($i = 0; $i < count($msg->cc); $i++) {
      $cc_array[$i][name] = ($msg->cc[$i]->personal ? $this->decode_header_string($msg->cc[$i]->personal) : $msg->cc[$i]->mailbox . '@' . $msg->cc[$i]->host);
      $cc_array[$i][address] = $msg->cc[$i]->mailbox . '@' . $msg->cc[$i]->host;
    }
    $out = array(
      'date' => $msg->date,
      'udate' => $msg->udate + $config['imap_timezone_offset'] * 60,
      'subject' => ($msg->subject ? $this->decode_header_string($msg->subject) : '[No Subject]'),
      'from_name' => ($msg->from[0]->personal ? $this->decode_header_string($msg->from[0]->personal) : $msg->from[0]->mailbox . '@' . $msg->from[0]->host),
      'from_address' => ($msg->reply_to[0]->mailbox ? $msg->reply_to[0]->mailbox . '@' . $msg->reply_to[0]->host : $msg->from[0]->mailbox . '@' . $msg->from[0]->host),
      'to_array' => $to_array,
      'cc_array' => $cc_array
    );
    return $out;
  }

  function set_replied($msgs) {
    for($i = 0; $i < count($msgs); $i++) {
      imap_setflag_full($this->mbox, $msgs[$i], '\\Answered', SE_UID);
    }
  }

  function delete_messages($msgs) {
    for($i = 0; $i < count($msgs); $i++) {
      imap_delete($this->mbox, $msgs[$i], FT_UID);
    }
    if($this->config['is_pop3']) {
      $this->expunge_messages();
    }
  }

  function move_messages($old_folder, $msgs, $new_folder) {
    if($old_folder != $new_folder) {
      $this->select_folder($old_folder);
      $msgss = implode(',', $msgs);
      imap_mail_move($this->mbox, $msgss, $this->create_imap_string_short($new_folder), CP_UID);
    }
  }

  function undelete_messages($msgs) {
    for($i = 0; $i < count($msgs); $i++) {
      imap_undelete($this->mbox, $msgs[$i], FT_UID);
    }
  }

  function expunge_messages() {
    imap_expunge($this->mbox);
  }

  function find_next_prev_uid($nowuid) {
    $msg_array = imap_sort($this->mbox, SORTARRIVAL, 1, SE_UID);
    $num_messages = count($msg_array);
    $res = bc_array_search($nowuid, $msg_array);
    if(($res == 0) && ($msg_array[$res] != $nowuid)) {
      return false;
    }
    if($res == 0) {
      $prev = -1;
    } else {
      $prev = $msg_array[$res - 1];
    }
    if($res == (count($msg_array) - 1)) {
      $next = -1;
    } else {
      $next = $msg_array[$res + 1];
    }
    return array($next, $prev, $res + 1, $num_messages);
  }

  function retrieve_message_list($offset = 1, $return = 5) {
    global $config;

    $out = array();
    $msg_array = imap_sort($this->mbox, SORTARRIVAL, 1, SE_UID);
    $msg_array = array_slice($msg_array, $offset - 1, $return);
    for($i=0; $i < count($msg_array); $i++) {
      $msg = imap_header($this->mbox, imap_msgno($this->mbox, $msg_array[$i]));
      $out[$i] = array(
        'msgno' => $msg_array[$i],
        'date' => $msg->date,
        'subject' => ($msg->subject ? $this->decode_header_string($msg->subject) : '[No Subject]'),
        'from' => ($msg->from[0]->personal ? $this->decode_header_string($msg->from[0]->personal) : $msg->from[0]->mailbox . '@' . $msg->from[0]->host),
        'fromaddress' => $msg->from[0]->mailbox . '@' . $msg->from[0]->host,
        'to' => ($msg->to[0]->personal ? $this->decode_header_string($msg->to[0]->personal) : $msg->to[0]->mailbox . '@' . $msg->to[0]->host),
        'toaddress' => $msg->to[0]->mailbox . '@' . $msg->to[0]->host,
        'size' => $msg->Size,
        'udate' => $msg->udate + $config['imap_timezone_offset'] * 60,
        'unread' => ($msg->Unseen == 'U' || $msg->Recent == 'N'),
        'deleted' => ($msg->Deleted == 'D'),
        'replied' => ($msg->Answered == 'A')
      );
      if(!$this->config['is_pop3']) {
        $partcount = $this->parts_count($msg_array[$i]);
        $out[$i]['count_mime'] = $partcount['mime'];
        $out[$i]['count_disposition'] = $partcount['disposition'];
      }
    }
    return $out;
  }
  
  function select_folder($folder) {
    imap_reopen($this->mbox, $this->create_imap_string($folder));
  }

  function retrieve_message_headers_text($folder, $msgnum) {
    $this->select_folder($folder);
    return imap_fetchheader($this->mbox, $msgnum, FT_UID);
  }

  function retrieve_message_body($folder, $msgnum) {
   $this->select_folder($folder);
   return imap_body($this->mbox, $msgnum, FT_UID);
  }

  function create_mailbox($folder){
   $string=$this->create_imap_string($folder);
   if(!imap_createmailbox($this->mbox,$string)){
    $this->list_errors($string);
   }
  }

  function delete_mailbox($folder){
   $string=$this->create_imap_string($folder);
   if(!imap_deletemailbox($this->mbox,$string)){
    $this->list_errors($string);
   }
  }

  function rename_mailbox($oldfolder, $newfolder){
   $string1=$this->create_imap_string($oldfolder);
   $string2=$this->create_imap_string($newfolder);
   if(!imap_renamemailbox($this->mbox,$string1,$string2)){
    $this->list_errors($string1);
   }
  }

 function retrieve_all_headers_array($folder, $msgnum) {
    $this->select_folder($folder);
    $headers_text = imap_fetchheader($this->mbox, $msgnum, FT_UID);
    $headers_text = preg_replace("/\r/", "\n", $headers_text);
    $headers_text = preg_replace("/\n+/", "\n", $headers_text);
    $headers_array = preg_split("/\n/", $headers_text);
    $headers = array();
    for($i = 0; $i < count($headers_array); $i++) {
      if($headers_array[$i]) {
        if(preg_match("/^\s+/", $headers_array[$i])) {
          $headers[strtolower($lastname)] .= " " . preg_replace("/^\s+/", "", $headers_array[$i]);
        } else {
          list($name, $value) = preg_split("/:\s+/", $headers_array[$i], 2);
          $headers[strtolower($name)] = $this->decode_header_string($value);
          $lastname = $name;
        }
      }
    }
    return $headers;
  }

  function decode_header_string($hed_str) {
    if($hed_str) {      
      if(substr($hed_str, 0, 2) == "=?") {
        $start_pos = strpos($hed_str, "?", 2);
        $type = substr($hed_str, $start_pos + 1, 1);
        $newstr = substr($hed_str, $start_pos + 3, strlen($hed_str) - ($start_pos + 5));
        if (strtoupper($type) == "Q") {
          $output = str_replace("_", " " , imap_qprint($newstr));
        } elseif (strtoupper($type) == "B") {
          $output = base64_decode($newstr);
        } else {
          $output = $newstr;
        }
      } else {
        $output = $hed_str;
      }
    } 
    return $output;
  }

 function last_error() {
  return imap_last_error();
 }

 function list_errors($imap_string) {
  $errorlist=$imap_string."\n(...)\n";
  foreach(imap_errors() as $dummy=>$string){
   $errorlist.=$string."\n";
  }
  setcookie("error",$errorlist);
 }
}

function pre_print_r($array) {
  print "<pre>";
  print_r($array);
  print "</pre><hr>\n";
}
?>
