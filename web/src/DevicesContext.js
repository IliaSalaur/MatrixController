import { createContext } from 'react';
// export const DevicesContext = createContext({selectedDevice:0, devices:[{id:123,h:16,w:16,tag:'hi', ip:'192.168.0.2'}]});
export const DevicesContext = createContext({selectedDevice:null, devices:[]});