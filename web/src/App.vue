<template>
  <div id="app">
      <section class="hero">
          <div class="hero-body">
              <div class="container">
                  <h1 class="title">
                      Charlie's Wifi - ArtNet - DMX gateway
                  </h1>
                  <div class="buttons">
                      <b-button type="is-primary" @click="$router.push('/')">Home</b-button>
                      <b-button type="is-primary" @click="$router.push('/laser')">Laser control</b-button>
                      <b-button type="is-primary" @click="$router.push('/configuration')">Configuration</b-button>
                      <b-button type="is-primary" @click="goto('/resetwifi')">Reset Wifi</b-button>
                      <b-button type="is-primary" @click="goto('/reset')">Reboot</b-button>
                  </div>
                  <router-view></router-view>
              </div>
          </div>
      </section>

  </div>
</template>

<script>

export default {
  name: 'app',
  components: {

  },
    methods: {
      goto: function(url) {
          window.location.href = url;
      }
    }
}
</script>

<style lang="scss">

</style>
