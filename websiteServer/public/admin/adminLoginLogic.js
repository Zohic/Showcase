
var submitForm = document.getElementById("adminForm");
submitForm.addEventListener('submit', AdminLogin);

function AdminLogin(e) {
    e.preventDefault();
    const inputs = submitForm.getElementsByTagName("input");

    const bodyToSend = JSON.stringify({
        "login": inputs[0].value,
        "password": inputs[1].value
    });


    fetch("/admin-login", {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: bodyToSend
    }).catch(err => console.log(err)).then((res) => { if (res.status == 200) { return res.text() } }).then((res) => {
        const respBody = JSON.parse(res);
        setCookie("APSSID", respBody.sessionID, {'max-age': 3600 });//secure:true - doesnt work without SSL, even declration of secure corrupts cookie
        document.location.replace(respBody.href);
    });
}


