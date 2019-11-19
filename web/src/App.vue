<template>
    <div id="app">
        <v-app id="inspire">
            <v-navigation-drawer
                    v-model="drawer"
                    app
                    clipped
            >
                <v-list dense>
                    <v-list-item
                            v-for="item in items"
                            :key="item.text"
                            @click="goto(item.location)"
                    >
                        <v-list-item-action>
                            <v-icon>{{ item.icon }}</v-icon>
                        </v-list-item-action>
                        <v-list-item-content>
                            <v-list-item-title>
                                {{ item.text }}
                            </v-list-item-title>
                        </v-list-item-content>
                    </v-list-item>
                </v-list>
            </v-navigation-drawer>

            <v-app-bar
                    app
                    clipped-left
                    color="red"
                    dense
            >
                <v-app-bar-nav-icon @click.stop="drawer = !drawer"></v-app-bar-nav-icon>
                <v-toolbar-title class="mr-5 align-center">
                    <span class="title">Charlie's Wifi - ArtNet - DMX gateway</span>
                </v-toolbar-title>
            </v-app-bar>

            <v-content>
                <router-view></router-view>
            </v-content>
        </v-app>
    </div>
</template>

<script>

    export default {
        name: 'app',
        components: {},
        data: () => ({
            drawer: null,
            items: [
                {icon: 'mdi-home', text: 'Home', location:'/'},
                {icon: 'mdi-floor-lamp-variant', text: 'Laser', location:'/laser'},
                {icon: 'mdi-cogs', text: 'Configuration', location:'/configuration'},
            ],
        }),
        methods: {
            goto: function (location) {
                this.$router.push(location)
            }
        }
    }
</script>

<style lang="scss">

</style>
