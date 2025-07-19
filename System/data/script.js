const BASE_URL="http://192.168.4.1";async function espFetch(path){const res=await fetch(`${BASE_URL}${path}`);return await res.json()}
class MiniZip{constructor(){this.files=[]}
file(name,content){const encoder=new TextEncoder();const data=encoder.encode(content);this.files.push({name,data})}
generate(options={type:"blob"}){const fileRecords=[];const centralDirectory=[];let offset=0;for(const file of this.files){const encodedName=new TextEncoder().encode(file.name);const localHeader=[0x50,0x4b,0x03,0x04,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,...this._u32(0),...this._u32(file.data.length),...this._u32(file.data.length),...this._u16(encodedName.length),0x00,0x00];const fileHeader=new Uint8Array([...localHeader,...encodedName,...file.data]);fileRecords.push(fileHeader);const centralHeader=[0x50,0x4b,0x01,0x02,0x0a,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,...this._u32(0),...this._u32(file.data.length),...this._u32(file.data.length),...this._u16(encodedName.length),0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,...this._u32(offset),...encodedName];centralDirectory.push(new Uint8Array(centralHeader));offset+=fileHeader.length}
const cdSize=centralDirectory.reduce((s,b)=>s+b.length,0);const eocd=new Uint8Array([0x50,0x4b,0x05,0x06,0x00,0x00,0x00,0x00,...this._u16(this.files.length),...this._u16(this.files.length),...this._u32(cdSize),...this._u32(offset),0x00,0x00]);const zipSize=offset+cdSize+eocd.length;const out=new Uint8Array(zipSize);let pos=0;for(const chunk of fileRecords){out.set(chunk,pos);pos+=chunk.length}
for(const chunk of centralDirectory){out.set(chunk,pos);pos+=chunk.length}
out.set(eocd,pos);return options.type==="blob"?new Blob([out],{type:"application/zip"}):out}
generateAsync(options={type:"blob"}){return Promise.resolve(this.generate(options))}
_u16(v){return[v&0xff,(v>>8)&0xff]}
_u32(v){return[v&0xff,(v>>8)&0xff,(v>>16)&0xff,(v>>24)&0xff]}}
const miniXLSX=(()=>{function sheetToXML(name,data){const rows=data.map((row,i)=>`<row r="${i + 1}">`+row.map((cell,j)=>{const col=String.fromCharCode(65+j);return `<c r="${col}${i + 1}" t="str"><v>${cell}</v></c>`}).join('')+`</row>`).join('');return `<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
  <sheetData>${rows}</sheetData>
</worksheet>`}
function workbookXML(sheetNames){const sheets=sheetNames.map((name,i)=>`<sheet name="${name}" sheetId="${i + 1}" r:id="rId${i + 1}"/>`).join('');return `<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
          xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
  <sheets>${sheets}</sheets>
</workbook>`}
function relsXML(sheetCount){const rels=Array.from({length:sheetCount},(_,i)=>`<Relationship Id="rId${i + 1}" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target="worksheets/sheet${i + 1}.xml"/>`).join('');return `<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  ${rels}
</Relationships>`}
function contentTypesXML(sheetCount){const overrides=Array.from({length:sheetCount},(_,i)=>`<Override PartName="/xl/worksheets/sheet${i + 1}.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"/>`).join('');return `<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  ${overrides}
  <Override PartName="/xl/workbook.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"/>
</Types>`}
function writeFile(sheets,filename="workbook.xlsx"){const zip=new MiniZip();sheets.forEach((sheet,i)=>{const xml=sheetToXML(sheet.name,sheet.data);zip.file(`xl/worksheets/sheet${i + 1}.xml`,xml)});zip.file("[Content_Types].xml",contentTypesXML(sheets.length));zip.file("xl/workbook.xml",workbookXML(sheets.map(s=>s.name)));zip.file("xl/_rels/workbook.xml.rels",relsXML(sheets.length));zip.file("_rels/.rels",`<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="xl/workbook.xml"/>
</Relationships>`);zip.generateAsync({type:"blob"}).then(blob=>{const a=document.createElement("a");a.href=URL.createObjectURL(blob);a.download=filename;a.style.display="none";document.body.appendChild(a);a.click();setTimeout(()=>{document.body.removeChild(a);URL.revokeObjectURL(a.href)},100)})}
return{writeFile}})();const regisOkBtn=document.getElementById("regis-ok-btn");const loginOkBtn=document.getElementById("login-ok-btn");const passOkBtn=document.getElementById("pass-ok-btn");const yesBtn=document.getElementById("asw-yes");const noBtn=document.getElementById("asw-no");const resultOkBtn=document.getElementById("result-del-ok-btn");const inforBtn=document.getElementById("view-infor-btn");const hisBtn=document.getElementById("his-btn");const regisForm=document.getElementById("regis-form");const regisMsgPage=document.getElementById("regis-mes-page");const regisMsg=document.getElementById("regis-message");const loginIdInput=document.getElementById("login-id");const rightIdText=document.getElementById("right-id-text");const idError=document.getElementById("id-error");const idErrorMsg=document.getElementById("id-error-msg");const passInput=document.getElementById("pass");const eyeIcon=document.getElementById('eye-icon');const passError=document.getElementById("pass-error");const passErrorMsg=document.getElementById("pass-error-msg");const askDelUser=document.getElementById("ask-del-user");const askMsg=document.getElementById("ask-msg");const resultDelUser=document.getElementById("result-del-user");const resultMsg=document.getElementById("result-msg");const exportMsg=document.getElementById("export-msg");const exportMsgText=document.getElementById("export-msg-text");let globalUserId=null;regisOkBtn.addEventListener("click",()=>{regisMsgPage.classList.add("hidden");regisForm.reset()});loginOkBtn.addEventListener("click",()=>{idError.classList.add("hidden");loginIdInput.value=""});passOkBtn.addEventListener("click",()=>{passError.classList.add("hidden");passInput.value=""});yesBtn.addEventListener("click",()=>{askDelUser.classList.add("hidden");checkDelUser()});noBtn.addEventListener("click",()=>{askDelUser.classList.add("hidden")});resultOkBtn.addEventListener("click",()=>{resultDelUser.classList.add("hidden")});inforBtn.addEventListener("click",()=>{showUserInfo(globalUserId)});hisBtn.addEventListener("click",()=>{viewHis(globalUserId)});function navigate(pageId){document.querySelectorAll("[class^='page']").forEach(p=>p.classList.add("hidden"));document.getElementById(pageId)?.classList.remove("hidden");if(pageId==="register")regisForm.reset();else if(pageId==="enter-id")loginIdInput.value="";else if(pageId==="admin"){passInput.value="";eyeIcon.textContent='🙈'}}
regisForm.addEventListener("submit",async(e)=>{e.preventDefault();const formData=new FormData(regisForm);const id=formData.get("id");const query=new URLSearchParams(formData).toString();try{const result=await espFetch(`/regis?${query}`);switch(result.status){case "ok":regisMsg.textContent=`✅ ID ${id} registered successfully !`;navigate("menu");break;case "exists":regisMsg.textContent=`⚠️ User ID ${id} already exists !`;break;default:regisMsg.textContent=`❌ Failed to register user: ${result.message || "Unknown error"}`}}catch{regisMsg.textContent="⚠️ Error connecting to server."}
regisMsgPage.classList.remove("hidden")});async function checkLogin(){const id=loginIdInput.value.trim();if(!id){idErrorMsg.textContent="⚠️ Please enter an ID !";idError.classList.remove("hidden");return}
const now=new Date();const dd=String(now.getDate()).padStart(2,'0');const mm=String(now.getMonth()+1).padStart(2,'0');const yyyy=now.getFullYear();const today=`${dd}-${mm}-${yyyy}`;try{const result=await espFetch(`/checkId?id=${encodeURIComponent(id)}&date=${today}`);if(result.exists){rightIdText.textContent=id;navigate("right-id")}else{idErrorMsg.textContent=`❌ User ID ${id} does not exist !`;idError.classList.remove("hidden")}}catch{idErrorMsg.textContent="⚠️ Error connecting to server.";idError.classList.remove("hidden")}}
function togglePassword(){const isHidden=passInput.type==='password';passInput.type=isHidden?'text':'password';eyeIcon.textContent=isHidden?'👀':'🙈'}
async function checkPass(){const pass=passInput.value.trim();if(!pass){passErrorMsg.textContent="⚠️ Please enter the password !";passError.classList.remove("hidden");return}
try{const result=await espFetch(`/checkPass?pass=${encodeURIComponent(pass)}`);if(result.fit){passInput.value="";loadUsers()}else{passErrorMsg.textContent="❌ Entered wrong password !";passError.classList.remove("hidden")}}catch{passErrorMsg.textContent="⚠️ Error connecting to server.";passError.classList.remove("hidden")}}
function renderUserItem(userId){const item=document.createElement('div');item.className='user-item';item.innerHTML=`
    <label class="user-label">${userId}</label>
    <div class="dropdown">
      <button class="menu-btn">⋮</button>
      <div class="dropdown-content">
        <button onclick="viewDetail('${userId}')">🔍 Detail</button>
        <button onclick="deleteUser('${userId}')">❌ Delete</button>
      </div>
    </div>
  `;return item}
async function loadUsers(){try{const data=await espFetch("/getUsers");const userList=document.getElementById("user-list");const totalLabel=document.getElementById("total-storage");userList.innerHTML="";totalLabel.classList.add("hidden");const users=data.users;const totalPercent=Number(data.total_percent).toFixed(2);if(!Array.isArray(users)||users.length===0){document.getElementById("has-user").classList.add("hidden");document.getElementById("no-user").classList.remove("hidden");navigate("no-user");return}
users.forEach(user=>userList.appendChild(renderUserItem(user.id)));totalLabel.textContent=`Data Usage: ${totalPercent}%`;totalLabel.classList.remove("hidden");document.getElementById("no-user").classList.add("hidden");document.getElementById("has-user").classList.remove("hidden");navigate("has-user")}catch(err){alert("⚠️ Error connecting to server.");console.error(err)}}
function viewDetail(userId){globalUserId=userId;const viewDetailDiv=document.getElementById("view-detail");const h2=viewDetailDiv.querySelector("h2");h2.textContent=`ID ${userId}`;navigate("view-detail")}
async function deleteUser(userId){globalUserId=userId;askMsg.textContent=`❓ Do you want to delete user ${userId}?`;askDelUser.classList.remove("hidden")}
async function checkDelUser(){const result=await espFetch(`/delUser?id=${encodeURIComponent(globalUserId)}`);const userList=document.getElementById("user-list");if(result.del){resultMsg.textContent=`✅ User ${globalUserId} has been deleted.`;loadUsers();if(userList.children.length===0){document.getElementById("has-user").classList.add("hidden");document.getElementById("no-user").classList.remove("hidden")}}else{resultMsg.textContent=`❌ Deletion failed for user ${globalUserId}.`}
resultDelUser.classList.remove("hidden")}
async function showUserInfo(userId){try{const user=await espFetch(`/getUserInfo?id=${encodeURIComponent(userId)}`);if(user.error){alert("⚠️ "+user.error);return}
document.getElementById("user-info-title").textContent=`ID ${user.id}`;document.getElementById("info-id").textContent=user.id;document.getElementById("info-name").textContent=user.name;document.getElementById("info-gender").textContent=user.gender;document.getElementById("info-age").textContent=user.age;document.getElementById("info-hospital").textContent=user.hospital;navigate("user-info-page")}catch(err){alert("⚠️ Error connecting to server.")}}
function viewHis(userID){espFetch(`/getUserHis?id=${userID}`).then(data=>{const dateList=document.getElementById("date-list");dateList.innerHTML="";if(data.history&&data.history.length>0){data.history.forEach(date=>{const btn=document.createElement("button");btn.className="btn date";btn.textContent=date;btn.onclick=()=>{document.querySelector("#view-date h2").textContent=`Date ${date}`;loadLessonData(globalUserId,date);navigate('view-date')};dateList.appendChild(btn)});document.querySelector("#has-his h2").textContent=`ID ${userID}`;navigate("has-his")}else{document.querySelector("#no-his h2").textContent=`ID ${userID}`;navigate("no-his")}}).catch(error=>{console.error("Error fetching history:",error);alert("Failed to fetch history data.")})}
async function loadLessonData(userId,date){try{const lessons=await espFetch(`/getDayLesson?id=${encodeURIComponent(userId)}&date=${encodeURIComponent(date)}`);const tableHTML=`<div class="table-container">
    <table>
      <thead>
        <tr>
          <th>No.</th>
          <th>Mode</th>
          <th>Level</th>
          <th>Time</th>
        </tr>
      </thead>
      <tbody>
        ${lessons.map(l => `<tr><td>${l.no}</td><td>${l.mode}</td><td>${l.level}</td><td>${l.time}'</td>
          </tr>
        `).join('')}
      </tbody>
    </table>
  </div>`;

document.getElementById("view-date").innerHTML=`
      <h2>Date ${date}</h2>
      ${tableHTML}
      <button class="btn back" onclick="navigate('has-his')">Back</button>
    `}catch(err){alert("⚠️ Error loading lesson data.")}}
async function exportData(){try{const userInfo=await espFetch(`/getUserInfo?id=${encodeURIComponent(globalUserId)}`);if(userInfo.error)throw new Error("User info error: "+userInfo.error);const hisData=await espFetch(`/getUserHis?id=${encodeURIComponent(globalUserId)}`);const dateList=hisData.history||[];const historyMap={};for(const date of dateList){const lessons=await espFetch(`/getDayLesson?id=${encodeURIComponent(globalUserId)}&date=${encodeURIComponent(date)}`);if(!Array.isArray(lessons))continue;historyMap[date.replace(/-/g,"_")]=lessons.map(l=>({no:l.no||"",mode:l.mode||"",level:l.level??"null",time:l.time||""}))}
exportToMultiSheetExcel(userInfo,historyMap);showExportMsg(globalUserId)}catch(err){alert("⚠️ Failed to export data.");console.error("❌ Export error:",err)}}
function exportToMultiSheetExcel(userInfo,historyMap){const sheets=[];const inforData=[["Id",userInfo.id??""],["Name",userInfo.name??""],["Gender",userInfo.gender??""],["Age",userInfo.age??""],["Hospital",userInfo.hospital??""]];sheets.push({name:"Infor",data:inforData});for(const date in historyMap){const lessons=historyMap[date];const sheetData=[["No.","Mode","Level","Time"],...lessons.map(l=>[String(l.no??""),String(l.mode??""),String(l.level??""),String(l.time??"")+"'"])];const sheetName=`Date_${date}`.replace(/[\\/*?:[\]]/g,"_").slice(0,31);sheets.push({name:sheetName,data:sheetData})}
const filename=`${userInfo.id}_exported_data.xlsx`;miniXLSX.writeFile(sheets,filename)}
function showExportMsg(userId){exportMsgText.textContent=`ℹ️ Exported ID ${userId} data.`;exportMsg.classList.remove("hidden")}
function closeExportMsg(){exportMsg.classList.add("hidden")}