const form = document.getElementById("myForm");
const submitForm = document.getElementById("orderForm");
const selectedServiceLabel = document.getElementById("selectedLabel");
const selectedServiceImage = document.getElementById("selectedImage");
submitForm.addEventListener('submit', SubmitOrder);

var selectedService = 0;

function SubmitOrder(e) {
    e.preventDefault();
    const inputs = submitForm.getElementsByTagName("input");
     
    const bodyToSend = JSON.stringify({
        "email": inputs[0].value,
        "phone": inputs[1].value,
        "name": inputs[2].value,
        "service": selectedService
    });
    console.log(bodyToSend);

    fetch("/submit-order", {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: bodyToSend
    }).then(res => res.text()).then(res => {
        closeForm();
        console.log(res)
    });

}


servicesInfo = {};

GetServicesList().then((res) => {
    servicesInfo = res
}).catch(err => console.log(err));


function MakeOrder(serviceId) {
    selectedService = serviceId;
    form.style.display = "block";
    selectedServiceLabel.innerHTML = servicesInfo.ServiceType[serviceId - 1].service_name;
    selectedServiceImage.setAttribute("src", document.location.origin + "/mainPage/images/services/" + servicesInfo.ServiceType[serviceId - 1].ascii_name+".jpg");
}

function closeForm() {
    form.style.display = "none";
}

