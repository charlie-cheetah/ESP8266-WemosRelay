import Vue from 'vue'
import VueRouter from 'vue-router'
import Buefy from 'buefy'
import 'buefy/dist/buefy.css'

Vue.use(Buefy)
Vue.config.productionTip = false
Vue.use(VueRouter)

import App from './App.vue'
import Home from './pages/Home'
import Laser from './pages/Laser'
import Configuration from './pages/Configuration'
import Update from './pages/Update'

const routes = [
    { path: '/', component: Home },
    { path: '/laser', component: Laser },
    { path: '/configuration', component: Configuration },
    { path: '/update', component: Update }
]

const router = new VueRouter({
    routes // short for `routes: routes`
})

new Vue({
  router,
  render: h => h(App),
}).$mount('#app')
