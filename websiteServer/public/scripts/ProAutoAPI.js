function GetServicesList() {
    const listProm = new Promise((resolve, reject) => {
        fetch("/get-services", {
            method: "GET",
            headers: {}
            }
        ).then((res) => {
            resolve(res.json())

        }).catch((err) => {
            reject(err);
        });
    });

    return listProm;
}

function GetOrdersList() {
    const listProm = new Promise((resolve, reject) => {
        fetch("/get-orders", {
            method: "GET",
            headers: {}
        }
        ).then((res) => {
            resolve(res.json())

        }).catch((err) => {
            reject(err);
        });
    });

    return listProm;
}

function FormatPhoneNumber(num) {
    let number = num;

    if (num.length == 11) {
        if (num.indexOf("+7") != -1 || num[0] != '8' || num[1] != '9')
            return "invalid phone number";

    } else if (num.length == 12) {
        if (num.indexOf("+7") == 0) {
            number = num.replace("+7", "8");
            if (number.length == 11) {
                if (number[1] != '9')
                    return "invalid phone number";
            }
                
        } else
            return "invalid phone number";

    } else if (num.length == 10) {
        if (num.indexOf("9") == 0) {
            number = String.prototype.concat("8", num);
        }
    } else {
        return "invalid phone number";
    }

    return number;

}


