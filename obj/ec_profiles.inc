<?php
$pfs[]=array(
 "provider"  => "GMail (imap)",
 "real_host" => "gmail.com",
 "imap_host" => "imap.gmail.com",
 "imap_port" => 993,
 "imap_mailpath" => "[Google Mail]/",
 "imap_mainbox"  => "INBOX",
 "imap_flags" => "/imap/ssl",
 "example" => "johndoe",
 "hint" => "johndoe excluding @gmail.com"
);
$pfs[]=array(
 "provider"  => "AOL / America Online (imap)",
 "real_host" => "aol.com",
 "imap_host" => "imap.aol.com",
 "imap_port" => 143,
 "imap_mailpath" => "mail/",
 "imap_mainbox"  => "INBOX",
 "example" => "myscreenname",
 "hint" => "Screenname excluding @aol.com"
);
$pfs[]=array(
 "provider"  => "fastmail",
 "real_host" => "fastmail.fm",
 "imap_host" => "fastmail.fm",
 "imap_port" => 143,
 "imap_mailpath" => "mail/",
 "imap_mainbox"  => "INBOX",
 "hint" => "johndoe@domain.tld"
);
$pfs[]=array(
 "provider"  => "gmx.(at|ch|de) (imap)",
 "real_host" => "gmx.net",
 "imap_host" => "imap.gmx.net",
 "imap_port" => 143,
 "imap_mailpath" => "",
 "imap_mainbox"  => "INBOX",
 "example" => "johndoe@gmx.tld",
 "hint" => "Customer ID or johndoe@gmx.tld"
);
$pfs[]=array(
 "provider"  => "gmx.(at|ch|de) (pop3)",
 "real_host" => "gmx.net",
 "imap_host" => "pop.gmx.net/pop3",
 "imap_port" => 110,
 "imap_mailpath" => "mail/",
 "imap_mainbox"  => "INBOX",
 "example" => "johndoe@gmx.tld",
 "hint" => "Customer ID or johndoe@gmx.tld"
);
$pfs[]=array(
 "provider"  => "web.de (imap)",
 "real_host" => "web.de",
 "imap_host" => "imap.web.de",
 "imap_port" => 143,
 "imap_mailpath" => "",
 "imap_mainbox"  => "INBOX",
 "use_sentbox" => 1,
 "hint" => "johndoe@web.de"
);
$pfs[]=array(
 "provider"  => "web.de (pop3)",
 "real_host" => "web.de",
 "imap_host" => "pop3.web.de/pop3",
 "imap_port" => 110,
 "imap_mailpath" => "mail/",
 "imap_mainbox"  => "INBOX",
 "hint" => "johndoe@web.de"
);
?>
