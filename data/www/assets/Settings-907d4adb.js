import{a as m}from"./axios-4a70c6fc.js";import{e as u,_,r as s,o as g,f as w,w as t,a as e,g as d}from"./index-7e5fd013.js";const h=u({wifi:{ssid:"lewin",password:"88888888"}}),b={data(){return{form:h}},mounted(){m.get("/api/v1/config").then(o=>this.form=o.data).catch(function(o){console.log(o)})},methods:{onSubmit(){console.log("submit: "+this.form),m.post("/api/v1/config",this.form,{headers:{"Content-Type":"application/json"}}).then(o=>{console.log(o.data)}).catch(o=>{console.log(o)})},onUpdate(){m.get("/api/v1/config").then(o=>this.form=o.data).catch(function(o){console.log(o)})}}};function S(o,l,V,x,n,r){const f=s("el-input"),a=s("el-form-item"),c=s("el-button"),p=s("el-form");return g(),w(p,{model:n.form,"label-width":"120px"},{default:t(()=>[e(a,{label:"SSID"},{default:t(()=>[e(f,{modelValue:n.form.wifi.ssid,"onUpdate:modelValue":l[0]||(l[0]=i=>n.form.wifi.ssid=i)},null,8,["modelValue"])]),_:1}),e(a,{label:"PASSWORD"},{default:t(()=>[e(f,{modelValue:n.form.wifi.password,"onUpdate:modelValue":l[1]||(l[1]=i=>n.form.wifi.password=i)},null,8,["modelValue"])]),_:1}),e(a,null,{default:t(()=>[e(c,{type:"primary",onClick:r.onSubmit},{default:t(()=>[d("Submit")]),_:1},8,["onClick"]),e(c,{onClick:r.onUpdate},{default:t(()=>[d("Update")]),_:1},8,["onClick"])]),_:1})]),_:1},8,["model"])}const v=_(b,[["render",S]]);export{v as default};
