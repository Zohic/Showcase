require('dotenv').config();
const path = require('path');

const Server = require("./Server.js");

const PORT = process.env.PORT || 3000;
const pubUrl = process.env.PUBLIC_URL || 'localhost:3000';
const baseUrl = process.env.BASE_URL || 'localhost:3000';

const pubDir = path.join(__dirname, process.env.PUBLIC_DIR);
const hidDir = path.join(process.env.HIDDEN_DIR);
const database = require('./database');

const sendMail = require('./emailer');
const cryptoString = require('crypto-random-string');



const server = new Server(baseUrl, pubDir, hidDir);
server.Listen(PORT);


server.responses.get('GET').set('/',
    function (req, res) {
        res.writeHead(302, { 'Location': '/mainPage/index.html' });
        res.end();
    }
);

server.responses.get('GET').set('/admin',
    function (req, res) {
        res.writeHead(302, { 'Location': '/admin/index.html' });
        res.end();
    }
);


server.responses.get('GET').set('/adminOffice',
    function (req, res) {
        res.writeHead(302, { 'Location': '/adminOffice/adminPage/index.html' });
        res.end();
    }
);

server.responses.get('GET').set('/mainPage',
    function (req, res) {
        res.writeHead(302, { 'Location': '/mainPage/index.html' });
        res.end();
    }
);

server.responses.get('GET').set('/add',
    function (req, res) {
        database.AddOrder(0, "zakhar", "233", "mail@mail");
        res.end();
    }
);

server.responses.get('GET').set('/confirm',
    function (req, res) {
        console.log("confirmed");
        const url = new URL(path.join(pubUrl, req.url));
 
        database.VerifyOrder(url.searchParams.get("id"));

        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end("заказ номер " + url.searchParams.get("id")+" подтвержден, ожидайте звонка");
    }
);

server.responses.get('GET').set('/discard',
    function (req, res) {
        console.log("discarded");
        const url = new URL(path.join(pubUrl, req.url));

        database.DisсardOrder(url.searchParams.get("id"));

        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end("заказ отменён");
    }
);


server.responses.get('GET').set('/get-services',
    function (req, res) {
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(JSON.stringify(database.ServicesInfo));
    }
);

server.responses.get('GET').set('/get-orders',
    function (req, res) {
        if (server.CheckAccess(req, true)) {

            console.log("access is confimed: proceed to get orders");
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            
            database.GetOrdersList().then(list => {
                console.log("ending response");
                res.end(JSON.stringify(list));
            }).catch(err => console.log(err));
            

        } else {
            console.log("access to orders prohibited");
            res.statusCode = 403;
            res.setHeader('Content-Type', 'text/plain');
            res.end('Only Admins have access to that information');
        }
        
    }
);

server.responses.get('POST').set('/submit-order',
    function (req, res) {
        let body = "";
        req.on("data", function (chunk) { body += chunk; })
        req.on("end", function () {
            console.log(body);
            const data = JSON.parse(body);
            const oId = cryptoString(12);
            database.AddOrder(data.service, data.name, data.phone, data.email, oId);

            linkOk = pubUrl + "/confirm?id=" + oId;
            linkNot = pubUrl + "/discard?id=" + oId;

            data.id = oId;

            sendMail(data, linkOk, linkNot);
            res.end();
        });
    }
);

server.responses.get('POST').set('/push-order',
    function (req, res) {
        console.log("push order");
        let body = "";
        req.on("data", function (chunk) { body += chunk; });
        req.on("end", function () {
            console.log(body);
            const updated = JSON.parse(body).updatedOrder;

            database.UpdateStatus(updated);
            res.statusCode = 200;
            res.end();
        });

        req.on("error", function (err) {
            res.statusCode = 500;
            res.setHeader('Content-Type', 'text/plain');
            res.end("Can't update status");
        });

    }
);

adminInfo = { login: "andrew", password: "mazada" };
server.responses.get('POST').set('/admin-login',
    function (req, res) {
        let body = "";
        req.on("data", function (chunk) { body += chunk; })
        req.on("end", function () {
            console.log(body);
            const data = JSON.parse(body);

            server.allowedSession.push(cryptoString(16));
            const allwd = server.allowedSession[server.allowedSession.length - 1];

            if (data.login != adminInfo.login || data.password != adminInfo.password) {
                res.writeHead(401, { 'Content-Type': 'text/plain; charset=utf-8' });
                res.end("неверный логин или пароль");
            } else {
                res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
                const sessResp = {
                    href: "/adminOffice/adminPage/index.html",
                    sessionID: allwd
                }
                res.end(JSON.stringify(sessResp));
            }
        });
    }
);


const aboutPlace = {
    place: "ул. Шевченко, дом 84, кв. 90",
    phone: "8 800 555-35-35",
    email: "somemail@mail.com"
};

server.renderPages.set("/mainPage/index.html", function (req, res) {
    return {
        page: path.basename(req.url),
        about: aboutPlace
    };
});


server.renderPages.set("/mainPage/service.html", function (req, res) {
    return {
        page: path.basename(req.url),
        about: aboutPlace,
        ServicesInfo: database.ServicesInfo
    };
});

server.renderPages.set("/mainPage/contact.html", function (req, res) {
    return {
        page: path.basename(req.url),
        about: aboutPlace,
    };
});

server.renderPages.set("/mainPage/about.html", function (req, res) {
    return {
        page: path.basename(req.url),
        about: aboutPlace,
    };
});








