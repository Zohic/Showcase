const ordersDiv = document.getElementById('orders-section');
const ordersListDiv = document.getElementById('orders-list');
const statusSelectDiv = document.getElementById('status-select');

const orders = [];

function RequireOrders() {
    orders.splice(0, orders.length);
    GetOrdersList().then(list => {
        for (let i = 0; i < list.length; i++)
            orders.push(list[i]);

    }).catch(err => console.log(err));
    
}

const statusList = ["не подтверждён", "в очереди", "в обработке", "готов"];
const statusColor = ['#ff0a00', '#ff6a00', '#17298d', '#0eb630'];
servicesInfo = {};

GetServicesList().then((info) => {
    servicesInfo = info;
    FillFreshOrders(orders);
}).catch(err => console.log(err));

function OrderLine(id, name, status, phone, email, orderId, clName) {
    return orderLine =
        "<div class='top-info'><p class='order-name cant-select'>" + id + ". " + name + "</p>" +
        "<p class='order-status cant-select'> статус: " + statusList[status] + "</p></div>" +
        "<div class='bot-info'><p>Номер клиента: " + phone + ", почта: " + email +
    "</p><p> Код заказа: " + orderId + "</p> <p> Имя клиента: " + clName + "<div/>";
}

function FillOrders(list) {
    ordersListDiv.innerHTML = '';
    for (let i = list.length - 1; i >= 0; i--) {

        let orderLine = ordersListDiv.appendChild(document.createElement('div'));
        orderLine.className = "order-line";
        
        orderLine.id = i;
        
        orderLine.innerHTML = OrderLine(
            list[i].id,
            servicesInfo.ServiceType[list[i].service_type - 1].service_name,
            list[i].order_status,
            FormatPhoneNumber(list[i].client_phone_number),
            list[i].client_email,
            list[i].order_id,
            list[i].client_name);

        orderLine.firstChild.firstChild.onclick = ExpandOrder.bind(orderLine);
        const statusEl = orderLine.firstChild.children[1];
        statusEl.style.color = statusColor[list[i].order_status];
        statusEl.onclick = ShowSatusList.bind(list[i]);
        
    }
}

function FillFreshOrders() {
    orders.splice(0, orders.length);
    GetOrdersList().then(list => {
        for (let i = 0; i < list.length; i++)
            orders.push(list[i]);

        FillOrders(orders);

    }).catch(err => console.log(err));
}

const states = ['200px', '50px'];

function ExpandOrder() {
    if (this.style.height == states[0])
        this.style.height = states[1];
    else
        this.style.height = states[0];
}

var selectedOrder = null;
var updatedOrders = new Map();

function ShowSatusList(e) {
    statusSelectDiv.style.setProperty("display", "inline");
    statusSelectDiv.style.setProperty("top", e.clientY+"px");
    statusSelectDiv.style.setProperty("left", e.clientX + "px");
    selectedOrder = this;
}

function PushOrder(ord) {
    
    const bodyToSend = JSON.stringify({
        updatedOrder: ord
    });

    console.log("pushing order");
    console.log(bodyToSend);

    fetch("/push-order", {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: bodyToSend
    }).then(res => {
       
        if (res.status == 200) {
            FillFreshOrders();
        }
        else {
            console.log("updating status error: ");
            console.log(res);
        }
    }).catch(err => console.log(err));
}

function SetStatus(s) {
    
    selectedOrder.order_status = s;
    statusSelectDiv.style.setProperty("display", "none");
    PushOrder(selectedOrder);
    selectedOrder = null;
}

document.addEventListener('click', function (e) {

    let found = e.path.findIndex((el) => {
        
        let found = false;
        if (el.classList!=undefined && el.classList.contains('order-status'))
            found = true;

        return found;

    });

    if (found==-1)
        statusSelectDiv.style.setProperty("display", "none");
});

