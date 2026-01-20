
const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<title>Music Player</title>

<style>
* {
  box-sizing: border-box;
}

body {
  margin: 0;
  background: #121212;
  color: white;
  font-family: Arial, Helvetica, sans-serif;
  display: flex;
  justify-content: center;
  min-height: 100vh;
}


#app {
  width: 100%;
  max-width: 420px;
  padding: 20px;
}


h3 {
  margin-top: 0;
  margin-bottom: 15px;
  text-align: center;
}


#uploadBox {
  border: 2px dashed #1db954;
  border-radius: 12px;
  padding: 20px;
  text-align: center;
  margin-bottom: 20px;
}

#uploadBox input {
  display: none;
}

#uploadBox label {
  display: block;
  cursor: pointer;
}

#uploadBox button {
  margin-top: 10px;
  padding: 10px 18px;
  border-radius: 20px;
  border: none;
  background: #1db954;
  color: black;
  font-weight: bold;
  cursor: pointer;
}


#files {
  list-style: none;
  padding: 0;
  margin: 0;
}

#files li {
  display: flex;
  justify-content: space-between;
  align-items: center;
  background: #1e1e1e;
  padding: 10px 12px;
  border-radius: 10px;
  margin-bottom: 8px;
}

#files li.active {
  background: #1db954;
  color: black;
}

#files button {
  background: transparent;
  border: none;
  color: #ff5c5c;
  font-size: 18px;
  cursor: pointer;
}

#selectedFile {
  margin-bottom: 10px;
  padding: 8px;
  background: #1e1e1e;
  border-radius: 8px;
  font-size: 14px;
}

progress {
  height: 14px;
  border-radius: 10px;
}

#percent {
  margin-top: 6px;
  font-size: 13px;
}


#control {
  margin: 20px 0;
  text-align: center;
}

#control button {
  width: 70px;
  height: 70px;
  margin: 10px;
  font-size: 30px;
  border-radius: 15px;
  border: none;
  background: #1db954;
  color: black;
  font-weight: bold;
  cursor: pointer;
}

#mainLayout {
  display: flex;
  gap: 20px;
  width: 100%;
  max-width: 900px;
  padding: 20px;
}


#app {
  flex: 1;
  max-width: 420px;
}


#controlPanel {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
}


#control {
  margin-top: 20px;
  text-align: center;
}

#musicToggle {
  margin: 15px 0;
  text-align: center;
}

#musicToggle button {
  padding: 12px 30px;
  border-radius: 25px;
  border: none;
  background: #1db954;
  font-weight: bold;
  font-size: 16px;
  cursor: pointer;
}

#musicToggle button.playing {
  background: #ff5252;
  color: white;
}

#musicToggle button:active {
  transform: scale(0.95);
}


#control .row {
  display: flex;
  justify-content: center;
  gap: 12px;
}

</style>

</head>

<body>

<div id="mainLayout">

  
  <div id="app">
    <h3>🎵 Quản lý nhạc (SD Card)</h3>

    <div id="uploadBox">
      <label for="file">📂 Kéo thả hoặc bấm để chọn file</label>
      <input type="file" id="file" accept=".mp3,.wav">

      <progress id="progressBar" value="0" max="100" style="width:100%"></progress>
      <div id="percent">0%</div>

      <button onclick="upload()">Upload</button>
    </div>

    <div id="musicToggle">
      <button id="playPauseBtn" onclick="toggleMusic()">▶ Phát</button>
    </div>

    <div id="selectedFile">📂 Chưa chọn file</div>
    <ul id="files"></ul>
  </div>

  
  <div id="controlPanel">
    <h3>🤖 Điều khiển robot</h3>

    <div id="control">
    <div class="row">
      <button id="btn-turn-left" onclick="sendDir('TURN_LEFT')">↺</button>
      <button id="btn-up" onclick="sendDir('UP')">⬆️</button>
      <button id="btn-turn-right" onclick="sendDir('TURN_RIGHT')">↻</button>
    </div>

    <div class="row">
      <button id="btn-left" onclick="sendDir('LEFT')">⬅️</button>
      <button id="btn-down" onclick="sendDir('DOWN')">⬇️</button>
      <button id="btn-right" onclick="sendDir('RIGHT')">➡️</button>
    </div>

  </div>
</div>
</body>

<script>

const fileInput = document.getElementById("file");
const selectedText = document.getElementById("selectedFile");

fileInput.addEventListener("change", () => {
  if (fileInput.files.length > 0) {
    selectedText.textContent =
      "📤 Sẽ upload: " + fileInput.files[0].name;
  } else {
    selectedText.textContent = "📂 Chưa chọn file";
  }
});

function addUploaded(title) {
  const li = document.createElement("li");
  li.textContent = title;
  document.getElementById("uploaded").appendChild(li);
}



function upload() {
  const f = fileInput.files[0];
  if (!f) return alert("Chưa chọn file");

  const form = new FormData();
  form.append("file", f);

  const xhr = new XMLHttpRequest();
  xhr.open("POST", "/upload", true);

  
  xhr.upload.onprogress = (e) => {
    if (e.lengthComputable) {
      const percent = Math.round((e.loaded / e.total) * 100);

      document.getElementById("progressBar").value = percent;
      document.getElementById("percent").textContent = percent + "%";
    }
  };

  xhr.onload = () => {
    if (xhr.status === 200) {
      document.getElementById("percent").textContent = "✅ Upload xong";
      document.getElementById("progressBar").value = 100;

      setTimeout(loadFiles, 500); 

      fileInput.value = "";
      selectedText.textContent = "📂 Chưa chọn file";
    } else {
      alert("Upload lỗi");
    }
  };

  xhr.onerror = () => {
    alert("Lỗi mạng khi upload");
  };

  xhr.send(form);
}



let currentFile = "";

function loadFiles() {
  fetch("/list?ts=" + Date.now())
    .then(r => r.json())
    .then(files => {
      const ul = document.getElementById("files");
      ul.innerHTML = "";

      files.forEach(name => {
        const li = document.createElement("li");
        li.textContent = name;

        li.onclick = () => selectFile(name, li);

        const delBtn = document.createElement("button");
        delBtn.textContent = "❌";
        delBtn.onclick = (e) => {
          e.stopPropagation();
          del(name);
        };

        li.appendChild(delBtn);
        ul.appendChild(li);
      });
    });
}

function selectFile(name, el) {
  currentFile = name;

  document.getElementById("selectedFile").textContent =
    "🎵 Đã chọn: " + name;

  document.querySelectorAll("#files li")
    .forEach(li => li.classList.remove("active"));

  el.classList.add("active");

   
  const btn = document.getElementById("playPauseBtn");
  btn.textContent = "▶ Phát";
  btn.classList.remove("playing");
  isPlaying = false;

  
  fetch("/music?cmd=select&file=" + encodeURIComponent(name));
}


function del(name) {
  fetch("/delete?file=" + encodeURIComponent(name))
    .then(() => loadFiles());
}


function sendDir(dir) {
  console.log("Send:", dir);
  fetch("/arrow?dir=" + dir).catch(err => console.error(err));
}

document.addEventListener("keydown", e => {
  if (e.repeat) return; 

  let dir = null;
  switch(e.key) {
    case "ArrowUp":    dir = "UP"; break;
    case "ArrowDown":  dir = "DOWN"; break;
    case "ArrowLeft":  dir = "LEFT"; break;
    case "ArrowRight": dir = "RIGHT"; break;
  }

  if (dir) sendDir(dir);
});


let isPlaying = false;


function toggleMusic() {
  if (!currentFile) {
    alert("Chưa chọn file");
    return;
  }

  const btn = document.getElementById("playPauseBtn");

  if (!isPlaying) {
    
    fetch("/music?cmd=play&file=" + encodeURIComponent(currentFile));
    btn.textContent = "⏹ Dừng";
    btn.classList.add("playing");
    isPlaying = true;
  } else {
    
    fetch("/music?cmd=stop&file=" + encodeURIComponent(currentFile));
    btn.textContent = "▶ Phát";
    btn.classList.remove("playing");
    isPlaying = false;
  }
}


loadFiles();


</script>

</body>
</html>

)rawliteral";
