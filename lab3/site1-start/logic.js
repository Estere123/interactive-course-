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



//Language change 

const languageSelect = document.querySelector("#language-select"); 




//fetching data from lang.json file

fetch("./lang.json")
.then(function(resp) {
return resp.json();
})
.then(function(data) {
    console.log(data);
    setupLanguageSwitcher(data);

    const savedLanguage = localStorage.getItem("selectedLanguage");
    if (savedLanguage) {
        languageSelect.value = savedLanguage;
        applyTranslations(data[savedLanguage]);
    } else {
        applyTranslations(data["en"]);
    }
});


//Refrencing HTML elements
//index html 
const headerTittle = document.querySelector('header h1');
const navHome = document.querySelector("#nav-home");
const navProducts = document.querySelector("#nav-products");
const navAbout = document.querySelector("#nav-about");

const themeToggle = document.querySelector("#theme-toggle");
const heroTitle = document.querySelector(".hero h2");
const heroText = document.querySelector(".hero p")
const shopNowBtn = document.querySelector(".hero .btn")

const footerText = document.querySelector("footer p");



//About us html 
const aboutUsTitle = document.querySelector(".about>h2");
// console.log("ABOUT US TITLE : ",aboutUsTitle);
const aboutUsText = document.querySelector(".about p");


//Products html




function setupLanguageSwitcher(data) {
    languageSelect.addEventListener("change", function() {
        const selectedLanguage = this.value;

        localStorage.setItem("selectedLanguage", selectedLanguage);
        applyTranslations(data[selectedLanguage]);

    
    });
}

function changeText(element, text){
    try{
        element.textContent = text;
    }catch(e){
        console.error("Error: ", e);
    }
}

function applyTranslations(translations) {
    translateProductsPage(translations);
    headerTittle.textContent = translations.title;

    navHome.textContent = translations.home;
    navProducts.textContent = translations.products;
    navAbout.textContent = translations.about;
     
    themeToggle.textContent = translations.themeToggle;
    changeText(heroTitle, translations.welcomeMessage);
    changeText(heroText, translations.welcomeMessage);
    changeText(shopNowBtn, translations.shopNow);
    footerText.textContent = translations.footerText;


    aboutUsTitle.textContent = translations.aboutUsTitle;
    changeText(aboutUsText, translations.aboutUsText);


    function translateProductsPage(translations) {
     
        const isProductsPage = document.querySelector('.filters>h2') !==  null;

        if (!isProductsPage) {
            return;
        }
   
        const productsElements = {
            filters: document.querySelector(".filters>h2"),
            
            priceLow: document.querySelector('#price-sort option[value="price-low"]'),
            
            priceHigh: document.querySelector('#price-sort option[value="price-high"]'),
            
            addToCart: document.querySelectorAll(".product-grid .card button"),
           
            }
            
            if (productsElements.filters) productsElements.filters.textContent = translations.filters;
            if (productsElements.priceLow) productsElements.priceLow.textContent = translations.priceLow;
            if (productsElements.priceHigh) productsElements.priceHigh.textContent = translations.priceHigh;
        
            productsElements.addToCart.forEach((button) => {
                button.textContent = translations.addToCart;
            });
        
        }
      
    }









