const nodemailer = require("nodemailer");

const transporter = nodemailer.createTransport({
    host: "smtp.mail.ru",
    port: 587,
    secure: false, // true for 465, false for other ports
    auth: {
        user: "forsteamz@mail.ru", // generated ethereal user
        pass: "RCi1aTun4fD7LXDVH6q1" // generated ethereal password
    }
}, {
    from: "Авто сервис <forsteamz@mail.ru>"
}
);

function sendMail(data, linkOk, linkNot) {
    transporter.sendMail({
        to: data.email,
        subject: "Подтвердите заказ",
        text: "с вас пять тыщ",
        html: "<h2>Здравствуйте, " + data.name + "!</h2><br>" + "<h3>код вашего заказа: " + data.id +"</h3>"+
            "<h3>пожалуйста, подтвердите или отмените заказ</h3><br><a href='" + linkOk + "'>Подтвердить заказ</a><br><a href='" + linkNot + "'>Отменить заказ</a>"
        
    }, function (err, info) {
            if (err) return console.log(err);
            else console.log("sent " + info);
    });
}



module.exports = sendMail;

