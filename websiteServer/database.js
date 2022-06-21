const { Sequelize, Model, DataTypes } = require('sequelize');
const { randomUUID } = require('crypto');

const sequelize = new Sequelize('MainDataBase', 'null', 'null', {
    dialect: "sqlite",
    host: './mainDB.sqlite'
});


function AddOrder(type, clientName, phone, email, orderId) {
    Order.create({
        service_type: type,
        order_id: orderId,
        client_email: email,
        client_phone_number: phone,
        client_name: clientName,
        order_status: 0

    }).then(result => {
        console.log("created order: " + result);
    }).catch(err => {
        console.log(err);
    });
}

function VerifyOrder(orderId) {
    Order.update(
        {
            order_status: 1
        },
        {
            where: { order_id: orderId }
        }
    ).catch(err => console.log("verifying error: " + err));
}

function UpdateStatus(updated) {
    Order.update(
        {
            order_status: updated.order_status
        },
        {
            where: { order_id: updated.order_id }
        }
    ).catch(err => console.log("updating error: " + err));
}

function DisсardOrder(orderId) {
    Order.destroy(
        {
            where: { order_id: orderId }
        }
    ).catch(err => console.log("discarding error: " + err));
}

function GetOrdersList() {
    console.log("getting orders...");
    return Order.findAll();
}

class Order extends Model { }
class Service extends Model { }

Order.init({
    service_type: {
        type: DataTypes.INTEGER,
        allowNull: false
    },
    order_id: {
        type: DataTypes.STRING,
        allowNull: false
    },
    client_email: {
        type: DataTypes.STRING,
        allowNull: false
    },
    client_phone_number: {
        type: DataTypes.STRING,
        allowNull: false
    },
    client_name: {
        type: DataTypes.STRING,
        allowNull: false
    },
    order_status: {
        type: DataTypes.INTEGER,
        allowNull: false
    }
},
    {
        sequelize,
        modelName: 'order'
    }
);

Service.init({
    variation_id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        unique: true
    },
    price: {
        type: DataTypes.INTEGER,
        allowNull: false
    },
    service_section_id: {
        type: DataTypes.INTEGER,
        allowNull: false
    },
    service_name: {
        type: DataTypes.STRING(50),
        allowNull: false,
        unique: true
    },
    short_description: {
        type: DataTypes.STRING(100),
        allowNull: false
    },
    long_description: {
        type: DataTypes.STRING,
        allowNull: false
    },
    ascii_name: {
        type: DataTypes.STRING(20),
        allowNull: false,
        unique: true
    }
}, {
        sequelize,
        modelName: 'service',
        timestamps: false
});


const ServiceSection = [
    "Диагностика",//0
    "Трансмиссия",//1
    "Подвеска",//2
    "Тормозная система",//3
    "Двигатель",//4
    "Электроника",//5
    "Кузовной ремонт"//6
];
const ServiceStatus = [
    "не подтвержден",
    "в очереди",
    "обрабатывается",
    "готов"
];

const ServiceType = [];

sequelize.sync({ alter: false }).then(() => {
    console.log("db succesfully opened");

    Service.findAll().then(res => {
        const dataVals = res.forEach((item) => {
            ServiceType.push(item.dataValues);
        });

        
        
    }).catch(err => console.log(err));

    
}).catch(err => { console.log(err) });



module.exports = {
    AddOrder: AddOrder,
    VerifyOrder: VerifyOrder,
    DisсardOrder: DisсardOrder,
    GetOrdersList: GetOrdersList,
    UpdateStatus: UpdateStatus,
    ServicesInfo: {
        ServiceSection: ServiceSection,
        ServiceStatus: ServiceStatus,
        ServiceType: ServiceType
    }
};