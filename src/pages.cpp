#include "pages.hpp"

namespace Pages
{
  const char *configPage = R"=====(
    <!DOCTYPE html>
<html>
  <head>
    <title>Mode Selection</title>
    <style>
      body {
        font-family: Vazirmatn, Arial, sans-serif;
      }

      .container {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
      }

      .card {
        flex: 0 0 200px;
        height: 200px;
        margin: 10px;
        padding: 10px;
        border: 1px solid #ccc;
        border-radius: 5px;
        text-align: center;
        cursor: pointer;
      }

      .card:hover {
        background-color: #f5f5f5;
      }

      .selected {
        background-color: #e0e0e0;
      }

      .submit-container {
        display: flex;
        justify-content: center;
        margin-top: 20px;
      }

      .submit-btn {
        padding: 10px 75px;
        background-color: #4caf50;
        color: #fff;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        font-size: 18px;
        font-family: Vazirmatn;
        text-align: center;
      }

      .submit-btn:hover {
        background-color: #45a049;
      }
    </style>
  </head>
  <body>
    <h1 style="text-align: center">Select Operating Mode</h1>

    <div class="container">
      <div class="card selected" onclick="selectMode('0')">
        <h2>Mode 1</h2>
        <p>Operates locally and accumulative</p>
      </div>

      <div class="card" onclick="selectMode('1')">
        <h2>Mode 2</h2>
        <p>Operates locally with hourly report</p>
      </div>

      <div class="card" onclick="selectMode('2')">
        <h2>Mode 3</h2>
        <p>Sends data to gateway</p>
      </div>

      <div class="card" onclick="selectMode('3')">
        <h2>Gateway</h2>
        <p>Acts as gateway</p>
      </div>
    </div>

    <form id="modeForm" action="/settings" method="get">
      <input
        type="hidden"
        id="selectedModeInput"
        name="selectedMode"
        value="0"
      />
      <div class="submit-container">
        <input class="submit-btn" type="submit" value="Submit" />
      </div>
    </form>

    <script>
      function selectMode(mode) {
        var selectedModeInput = document.getElementById("selectedModeInput");
        selectedModeInput.value = mode;

        var cards = document.getElementsByClassName("card");
        for (var i = 0; i < cards.length; i++) {
          cards[i].classList.remove("selected");
        }

        var clickedCard = event.currentTarget;
        clickedCard.classList.add("selected");
      }
    </script>
  </body>
</html>
  )=====";

  const char *configDone = R"=====(
        <!DOCTYPE html>
<html>
  <head>
    <title>Mode Selected</title>
    <style>
      body {
        font-family: Vazirmatn, Arial, sans-serif;
      }
    </style>
  </head>
  <body>
    <h1 style="text-align: center">Settings saved. You can close the window now.</h1>
  </body>
</html>
  )=====";
}
