function formsubmit(thisid,nextid){
 var nextone = nextid-(-1);
 document.getElementById('div'+thisid).style.display='block';

 document.upload.submit();
 document.upload.target='iframe'+nextid;
 newFrame = document.createElement("iframe");
 newFrame.name='iframe'+nextid;
 newFrame.id='iframe'+nextid;
 newFrame.scrolling = 'no';
 oldFrame = document.getElementById("iframe"+thisid);
 oldDiv = document.getElementById("div"+thisid);

 newDiv = document.createElement("div");
 newDiv.id='div'+nextid;
 newDiv.style.display='none';
 newDiv.innerHTML = '(#'+nextid+') \<b id="status'+nextid+'">\<img class="load" src="./tpl/light_blue/quota/load.gif"\/>\<\/b> \<span id="file'+nextid+'">...\<\/span>';

 document.getElementById('browse').setAttribute("onChange","formsubmit('"+nextid+"','"+nextone+"');");
 document.getElementById('hidden').setAttribute("value",nextid);
 document.upload.insertBefore(newDiv, oldDiv);
 document.body.insertBefore(newFrame, oldFrame);
 document.forms.upload.reset();
}