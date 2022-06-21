const http = require('http');
const fs = require('fs');
const path = require('path');
const ejs = require('ejs');

const mimeList = {
    html: 'text/html',
    txt: 'text/plain',
    css: 'text/css',
    gif: 'image/gif',
    jpg: 'image/jpeg',
    png: 'image/png',
    svg: 'image/svg+xml',
    js: 'application/javascript'
};

module.exports = class Server {

    constructor(bUrl, pubDir, hidDir) {
        this.server = http.createServer();

        this.responses = new Map();
        this.responses.set('GET', new Map());
        this.responses.set('POST', new Map());
        this.responses.set('PUT', new Map());
        this.responses.set('DELETE', new Map());

        this.renders = [];
        this.renderPages = new Map();

        this.baseUrl = bUrl;
        this.hiddenDir = hidDir;
        this.publicDir = pubDir;

        this.allowedSession = [];

        this.server.on('request', this.Response.bind(this));

    }
    Listen(port) {
        try {
            this.server.listen(port, () => { console.log("server listening at portik " + port); })
        } catch (e) {
            console.log(e);
        }
    }

    CheckAccess(req, debug=false) {
        const cooks = req.headers.cookie;

        if (debug) {
            console.log("checking access: ");
            console.log(req.headers);
        }
            

        if (!cooks) {
            return false;
        } else {
            const jar = cooks.split("; ");
            let found = false;

            for (let i = 0; i < jar.length; i++) {
                const cookie = jar[i].split('=');
                if (cookie[0] == "APSSID") {
                    if (this.allowedSession.indexOf(cookie[1]) != -1)
                        found = true;
                }
            }

            return found;
        }
    }


    Response(req, res) {
        const url = new URL(path.join(this.baseUrl, req.url));
        
        if (path.extname(url.pathname) == '') {
            const corResp = this.responses.get(req.method).get(url.pathname);

            if (corResp) {
                corResp(req, res);
            } else {
                res.statusCode = 501;
                res.setHeader('Content-Type', 'text/plain');
                return res.end("Can't help you");
            }
        } else {
            let allowFile = true;

            if (url.pathname.indexOf(this.hiddenDir) == 1)
            {
                allowFile = this.CheckAccess(req);
            }

            if (allowFile)
                this.PublicFileResponse(req, res, url.pathname);
            else {
                
                
                if (url.pathname.split('/')[1] == 'adminOffice') {
                    console.log("bruh");
                    
                    res.writeHead(302, { 'Location': '/admin' });
                    res.end();
                } else {
                    res.statusCode = 403;
                    res.setHeader('Content-Type', 'text/plain');
                    res.end('Forbidden');
                }
                
                
            }

        }
    }

    HiddenFileResponse(res, hidPath) {
        this.FileResponse(res, path.join(this.publicDir, this.hiddenDir, hidPath));
    }

    PublicFileResponse(req, res, publicPath) {
        //console.log("sending public file");

        const file = path.join(this.publicDir, publicPath);

        const funcForRedner = this.renderPages.get(publicPath);

        //if type is html and there is render for it
        if (mimeList[path.extname(file).slice(1)] == 'text/html' &&
            funcForRedner) {
            this.FileRenderResponse(res, file, funcForRedner(req, res));
            return;
        }

        if (file.indexOf(this.publicDir + path.sep) != 0) {
            res.statusCode = 403;
            res.setHeader('Content-Type', 'text/plain');
            return res.end('Forbidden');
        }

        this.FileResponse(res, file);
    }

    FileResponse(res, file) {
        const type = mimeList[path.extname(file).slice(1)] || 'text/plain';
        //console.log("sending " + file);

        const s = fs.createReadStream(file);
        s.on('open', function () {
            res.setHeader('Content-Type', type);
            res.statusCode = 200;
            s.pipe(res);
        });

        s.on('error', function () {
            res.setHeader('Content-Type', 'text/plain');
            res.statusCode = 404;
            console.log("can't find: " + file);
            res.end('Not found');
        });
    }



    FileRenderResponse(res, file, rendData) {
        //var content = fs.readFileSync(, 'utf8');
        ejs.renderFile(file, rendData, function (err, content) {
            if (err) {
                console.log("render err: " + err.message);
                res.setHeader('Content-Type', 'text/plain');
                res.statusCode = 404;
                res.end('Not found');
            }
            res.setHeader('Content-Type', 'text/html');
            res.end(content);
        });
        
    }

}

