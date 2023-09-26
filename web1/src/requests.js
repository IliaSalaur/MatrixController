// export const LOCATION = window.location.host;
export const LOCATION = 'localhost:80';

export function setEffect(efName, efProps, device){
    // let url = 'http://' + LOCATION + "/setEffect";

    let url = 'http://' + device.ip + "/setEffect";
    console.log(device)

    let xhr = new XMLHttpRequest();
    xhr.open("POST", url);
    xhr.setRequestHeader('Content-Type', 'application/json');

    let cleanProps = {...efProps};

    const keys = Object.keys(cleanProps);
    keys.forEach((el) =>{
        cleanProps[el] = String(cleanProps[el].def);
    });

    let data = {effect:efName, properties:cleanProps};
    console.log(data);

    xhr.send(JSON.stringify(data));
}

export function setTextTemplate(sequence, device){
    let url = 'http://' + LOCATION + "/setTextTemplate";
    // let url = 'http://' + device.ip + "/setEffect";
    console.log(device)

    let xhr = new XMLHttpRequest();
    xhr.open("POST", url);
    xhr.setRequestHeader('Content-Type', 'application/json');

    let cleanProps = [];

    sequence.forEach((template, i) => {
        const keys = Object.keys(template);
        cleanProps[i] = {...template, textFilterProps: {...template.textFilterProps}};
        keys.forEach(((propName)=> {
            switch(propName)
            {
                case 'textFilter':
                    cleanProps[i][propName] = Number(template[propName].def);
                    break;

                case 'textFilterProps':
                    console.log("filter props");
                    const filterProps = cleanProps[i][propName];
                    const filterKeys = Object.keys(filterProps);
                    filterKeys.forEach((key, keyI) =>{
                        cleanProps[i].textFilterProps[key] = String(cleanProps[i].textFilterProps[key].def);
                    });
                    break;

                default:
                    cleanProps[i][propName] = template[propName].def;
                    break;
            }
        }));
    });

    
    console.log(cleanProps);

    xhr.send(JSON.stringify({effect:"TextSequence", properties:cleanProps}));
}

export function setConfigs(configs, device)
{
    // let url = 'http://' + LOCATION + "/setConfigs";
    let url = 'http://' + device.ip + "/setConfigs";

    let xhr = new XMLHttpRequest();
    xhr.open("POST", url);
    xhr.setRequestHeader('Content-Type', 'application/json');

    const keys = Object.keys(configs);
    let newObj = {}

    keys.forEach((key) =>{
        newObj[key] = configs[key].def;
    });

    console.log(newObj);

    xhr.send(JSON.stringify(newObj));
}

export function registerAsChild(childDevice, device)
{
    let url = 'http://' + childDevice.ip + "/registerAsChild";

    let xhr = new XMLHttpRequest();
    xhr.open("POST", url);
    xhr.setRequestHeader('Content-Type', 'application/json');

    xhr.send(JSON.stringify(device));
}