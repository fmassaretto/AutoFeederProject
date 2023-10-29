let schedulerAddedTrack = 1;
var scheduleTimeArray = new Array();

      function atualizar() {
        $.ajax({
          type: "POST",
          contentType: "text/plain",
          url: window.location + "quiz",
          success: function (dados) {
            $("#temperatura").html(dados);
            window.setTimeout(atualizar, 60000);
          },
          erro: function (erro) {
            console.log("Erro: " + erro);
          },
        });
      }

      function sendScheduler(jsonData){
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

      function sendQuiz(result){
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

      $(document).ready(function () {
        let addSchedulerBtn = document.getElementById("addBtn");
        let quizForm = document.getElementById("quiz-form");
        let schedulerForm = document.getElementById("scheduler-form");
        let submitBtn = document.getElementById("submitBtn");
        let successMessage = document.getElementById("successMessage");
        let wrongAnswerMessage = document.getElementById("wrongAnswerMessage");

        successMessage.hidden = true;
        wrongAnswerMessage.hidden = true;

        addSchedulerBtn.addEventListener("click", (e) => {
          e.preventDefault();
          if(schedulerAddedTrack < 3){
            createSchedulerElement(schedulerAddedTrack);
          }
        });

        schedulerForm.addEventListener("submit", (e)=>{
          e.preventDefault();
          console.log(e.target);
          var data = new FormData(schedulerForm);
          scheduleTimeArray = new Array();

          data.forEach((value, key) =>{
            var scheduleTimeJson = new Object();
            scheduleTimeJson.key = key;
            scheduleTimeJson.value = parseInt(value);

            scheduleTimeArray.push(scheduleTimeJson);
          })
          // console.log("scheduleJsonObj");
          // console.log("--------------");
          // let scheduleJson = JSON.parse(JSON.stringify(scheduleTimeArray));
          console.log(scheduleTimeArray);
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
            submitBtn.disabled = true;
            console.log("Errado! Tente novamente!");
            sendQuiz("lose");
          } else {
            successMessage.hidden = false;
            wrongAnswerMessage.hidden = true;
            submitBtn.disabled = true;
            console.log("Parabens!!! Você ganhou");
            sendQuiz("won");
          }
        });

        quizForm.addEventListener("reset", (e) => {
          submitBtn.disabled = false;
          wrongAnswerMessage.hidden = true;
          successMessage.hidden = true;
        });
      });