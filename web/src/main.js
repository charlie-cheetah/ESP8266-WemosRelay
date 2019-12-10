import Vue from 'vue'
import VueRouter from 'vue-router'
import VueNativeSock from 'vue-native-websocket'

Vue.config.productionTip = false
Vue.use(VueRouter)

//Vue.prototype.$location = '192.168.1.102';
Vue.prototype.$location = window.location.host;

Vue.use(VueNativeSock, 'ws://' +  window.location.host + '/ws', {
    connectManually: true,
    reconnection: true // (Boolean) whether to reconnect automatically (false)
})

import App from './App.vue'
import Home from './pages/Home'
import Laser from './pages/Laser'
import Configuration from './pages/Configuration'
import vuetify from './plugins/vuetify';

const routes = [
    { path: '/', component: Home },
    { path: '/laser', component: Laser },
    { path: '/configuration', component: Configuration },
]

const router = new VueRouter({
    routes // short for `routes: routes`
})

new Vue({
    router,
    vuetify,
    render: h => h(App)
}).$mount('#app')
