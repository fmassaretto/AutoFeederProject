let schedulerAddedTrack = 1;
let scheduleTimeArray = new Array();

function sendScheduler(jsonData) {
  $.ajax({
    type: "POST",
    contentType: "application/json",
    url: window.location + "schedule/",
    data: JSON.stringify(jsonData),
    success: function (data) {
      console.log(data);
    },
    erro: function (erro) {
      console.log("Erro: " + erro);
    },
  });
}

function sendQuiz(result) {
  $.ajax({
    type: "POST",
    contentType: "text/plain",
    url: window.location + "quiz/result/" + String(result),
    success: function () {
      console.log("Enviado ");
    },
    erro: function (erro) {
      console.log("Erro: " + erro);
    },
  });
}

function checkBowlStatus() {
  $.ajax({
    type: "GET",
    contentType: "text/plain",
    url: window.location + "bowl-status/",
    success: function (data) {
      document.getElementById("bowl-status").innerText = data;
    },
    erro: function (erro) {
      console.log("Erro: " + erro);
    },
  });
}

function checkTankStatus() {
  $.ajax({
    type: "GET",
    contentType: "text/plain",
    url: window.location + "tank-status/",
    success: function (data) {
      document.getElementById("tank-status").innerText = data;
    },
    erro: function (erro) {
      console.log("Erro: " + erro);
    },
  });
}

function removeSchedulerElement(element) {
  element.preventDefault();
  const elementId = element.target.id;
  let elemtnToRemove = document.getElementById(elementId);
  const timeScheduler = document.getElementById("time-scheduler");
  timeScheduler.removeChild(elemtnToRemove);
  schedulerAddedTrack--;
}

function createSchedulerElement(count) {
  const timeScheduler = document.getElementById("time-scheduler");

  let schedulerElementID = "scheduler" + count;
  const scheduler = document.createElement("div");
  scheduler.id = schedulerElementID;
  scheduler.className = "scheduler";

  const hourContainer = document.createElement("div");
  hourContainer.className = "hour";
  const minuteContainer = document.createElement("div");
  minuteContainer.className = "minute";
  const secondContainer = document.createElement("div");
  secondContainer.className = "second";

  const hourLabel = document.createElement("label");
  hourLabel.htmlFor = "hour" + count;
  hourLabel.textContent = "Hora:";
  const hourInput = document.createElement("input");
  hourInput.type = "number";
  hourInput.id = "hour" + count;
  hourInput.name = "hour" + count;
  hourInput.min = "1";
  hourInput.max = "24";
  hourInput.required = true;

  hourContainer.appendChild(hourLabel);
  hourContainer.appendChild(hourInput);

  const minuteLabel = document.createElement("label");
  minuteLabel.htmlFor = "minute" + count;
  minuteLabel.textContent = "Minuto:";
  const minuteInput = document.createElement("input");
  minuteInput.type = "number";
  minuteInput.id = "minute" + count;
  minuteInput.name = "minute" + count;
  minuteInput.min = "0";
  minuteInput.max = "59";
  minuteInput.required = true;

  minuteContainer.appendChild(minuteLabel);
  minuteContainer.appendChild(minuteInput);

  const secondLabel = document.createElement("label");
  secondLabel.htmlFor = "second" + count;
  secondLabel.textContent = "Segundo:";
  const secondInput = document.createElement("input");
  secondInput.type = "number";
  secondInput.id = "second" + count;
  secondInput.name = "second" + count;
  secondInput.min = "0";
  secondInput.max = "59";
  secondInput.required = true;

  secondContainer.appendChild(secondLabel);
  secondContainer.appendChild(secondInput);

  scheduler.appendChild(hourContainer);
  scheduler.appendChild(minuteContainer);
  scheduler.appendChild(secondContainer);

  if (count > 0) {
    const removeBtn = document.createElement("button");
    removeBtn.className = "removeBtn btn btn-danger";
    removeBtn.id = schedulerElementID;
    removeBtn.textContent = "X";
    removeBtn.onclick = removeSchedulerElement;

    scheduler.appendChild(removeBtn);
  }

  timeScheduler.appendChild(scheduler);
  schedulerAddedTrack++;
}

function formatTime(time) {
  if (time > 9) {
    return time;
  }
  return "0" + time;
}

function functionTimeout() {
  const refresh = 1000; // Refresh rate in milli seconds
  mytime = setTimeout("refreshContents()", refresh);
}

function displayCurrentTime() {
  let today = new Date();
  let time =
    formatTime(today.getHours()) +
    ":" +
    formatTime(today.getMinutes()) +
    ":" +
    formatTime(today.getSeconds());

  document.getElementById("current-time").innerText = time;
}

function refreshContents() {
  checkTankStatus();
  checkBowlStatus();
  displayCurrentTime();
  functionTimeout();
}

function updateNextTimetable(arr) {
  if (!(arr instanceof Array)) throw Error("invalid argument: paramsList must be an array");

  if (arr.length == 0) {
    return;
  }

  let elemCount = 1;
  for (let i = 0; i < arr.length; i = i + 3) {
    const nextTimeElemId = "next-time-" + elemCount++;
    const nextTimeHour = arr[i].value;
    const nextTimeMinute = arr[i + 1].value;
    const nextTimeSecond = arr[i + 2].value;

    const nextTimeFull =
      formatTime(nextTimeHour) +
      ":" +
      formatTime(nextTimeMinute) +
      ":" +
      formatTime(nextTimeSecond);

    console.log(i);
    console.log(nextTimeElemId);
    console.log(nextTimeFull);

    document.getElementById(nextTimeElemId).innerText = nextTimeFull;
  }
}

$(document).ready(function () {
  let addSchedulerBtn = document.getElementById("addBtn");
  let quizForm = document.getElementById("quiz-form");
  let schedulerForm = document.getElementById("scheduler-form");
  let submitQuizBtn = document.getElementById("submitQuizBtn");
  let successMessage = document.getElementById("successMessage");
  let wrongAnswerMessage = document.getElementById("wrongAnswerMessage");

  successMessage.hidden = true;
  wrongAnswerMessage.hidden = true;

  addSchedulerBtn.addEventListener("click", (e) => {
    e.preventDefault();

    if (schedulerAddedTrack < 3) {
      createSchedulerElement(schedulerAddedTrack);
    }
  });

  schedulerForm.addEventListener("submit", (e) => {
    e.preventDefault();

    let data = new FormData(schedulerForm);
    scheduleTimeArray = new Array();

    data.forEach((value, key) => {
      let scheduleTimeJson = new Object();
      scheduleTimeJson.key = key;
      scheduleTimeJson.value = parseInt(value);

      scheduleTimeArray.push(scheduleTimeJson);
    });

    updateNextTimetable(scheduleTimeArray);
    sendScheduler(scheduleTimeArray);
  });

  quizForm.addEventListener("submit", (e) => {
    e.preventDefault();

    let question1RightAnswer = document.getElementById("question1Answer3");
    let question2RightAnswer = document.getElementById("question2Answer2");
    let question3RightAnswer = document.getElementById("question3Answer4");

    if (
      !question1RightAnswer.checked ||
      !question2RightAnswer.checked ||
      !question3RightAnswer.checked
    ) {
      wrongAnswerMessage.hidden = false;
      successMessage.hidden = true;
      submitQuizBtn.disabled = true;
      console.log("Errado! Tente novamente!");
      sendQuiz("lose");
    } else {
      successMessage.hidden = false;
      wrongAnswerMessage.hidden = true;
      submitQuizBtn.disabled = true;
      console.log("Parabens!!! Você ganhou");
      sendQuiz("won");
    }
  });

  quizForm.addEventListener("reset", (e) => {
    submitQuizBtn.disabled = false;
    wrongAnswerMessage.hidden = true;
    successMessage.hidden = true;
  });

  MQTTconnect();
  refreshContents();
});
