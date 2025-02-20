const themeBtn = document.querySelector("#theme-toggle");
const moodeLight = document.querySelector("#mode-light");
const moodeDark = document.querySelector("#mode-dark");
console.log(moodeDark, moodeLight)



const body = document.body;
if(localStorage.getItem("currentTheme") === "dark") {
    body.classList.add("dark-mode");
    moodeLight.style.display = "none";
    moodeLight.style.display = "inline";
}
else {
    moodeDark.style.display = "none";
    moodeDark.style.display = "inline";
}

themeBtn.addEventListener("click", () =>{
console.log("Button is clicked")
body.classList.toggle("dark-mode");
if (body.classList.contains ("dark-mode")) {
    //show light icon ad switch bacground colors
    moodeDark.style.display = "none";
    moodeDark.style.display = "inline";
    localStorage.setItem(currentTheme, "dark")
    currenttheme = "light";
}

else {
    //show dark icon and switch bacground color
    moodeLight.style.display = "none";
    moodeLight.style.display = "inline";
    localStorage.setItem(currentTheme, "light");
    currentTheme = "dark";
}

}) 