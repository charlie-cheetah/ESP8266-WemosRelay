<template>
    <div class="container">
        <h1 class="subtitle">Configuration</h1>
        <v-text-field
                label="Device Name"
                v-model="deviceName"
        ></v-text-field>
        <v-text-field
                label="Long device Name"
                v-model="longDeviceName"

        ></v-text-field>
        <v-text-field
                label="Universe"
                v-model="universe"
                type="number"
        ></v-text-field>
        <v-text-field
                label="Starting channel"
                v-model="startingChannel"
                type="number"
        ></v-text-field>
        <v-switch
                v-model="dmxEnabled"
                label="DMX enabled"
        ></v-switch>
        <v-btn color="primary" @click="saveSettings">Save settings</v-btn>
    </div>
</template>

<script>
    export default {
        name: "Configuration",
        data() {
            return {
                deviceName: '',
                longDeviceName: '',
                universe: 1,
                startingChannel: 0,
                dmxEnabled: true
            }
        },
        mounted() {
            let url = new URL('http://' + this.$location + '/info');
            fetch(url, {
            }).then(function(response){
                if (response.status !== 200) {
                    console.log('Looks like there was a problem. Status Code: ' +
                        response.status);
                    return;
                }

                // Examine the text in the response
                response.json().then(function(data) {

                    if (typeof data.device_name !== 'undefined') this.deviceName = data.device_name
                    if (typeof data.long_device_name !== 'undefined') this.longDeviceName = data.long_device_name
                    if (typeof data.universe !== 'undefined') this.universe = data.universe
                    if (typeof data.starting_channel !== 'undefined') this.startingChannel = data.starting_channel
                    if (typeof data.dmx_enabled !== 'undefined') this.dmxEnabled = data.dmx_enabled
                    console.log(typeof data.deviceName)
                }.bind(this));
            }.bind(this));
        },
        methods: {
            saveSettings() {
                let url = new URL('http://' + this.$location + '/settings');
                url.searchParams.append('deviceName', this.deviceName)
                url.searchParams.append('longDeviceName', this.longDeviceName)
                url.searchParams.append('universe', this.universe)
                url.searchParams.append('startingChannel', this.startingChannel)
                url.searchParams.append('dmxEnabled', this.dmxEnabled)
                fetch(url, {
                    method: 'POST'
                });
            }
        }
    }
</script>

<style scoped>

</style>