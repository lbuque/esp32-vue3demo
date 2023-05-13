<template>
  <el-form :model="form" label-width="120px">
    <el-form-item label="SSID">
      <el-input v-model="form.wifi.ssid" />
    </el-form-item>
    <el-form-item label="PASSWORD">
      <el-input v-model="form.wifi.password" />
    </el-form-item>
    <el-form-item>
      <el-button type="primary" @click="onSubmit">Submit</el-button>
      <el-button @click="onUpdate">Update</el-button>
    </el-form-item>
  </el-form>
</template>

<script lang="ts">
import { reactive } from "vue";
import axios from "axios";

// do not use same name with ref
const form = reactive({
  wifi: {
    ssid: "lewin",
    password: "88888888",
  },
});

export default {
  data() {
    return {
      form: form,
    };
  },
  mounted() {
    axios
      .get("/api/v1/config")
      .then((response) => (this.form = response.data))
      .catch(function (error) {
        console.log(error);
      });
  },
  methods: {
    onSubmit() {
      console.log("submit: " + this.form);
      axios
        .post("/api/v1/config", this.form, {
          headers: {
            "Content-Type": "application/json",
          },
        })
        .then((res) => {
          console.log(res.data);
        })
        .catch((Error) => {
          console.log(Error);
        });
    },
    onUpdate() {
      axios
        .get("/api/v1/config")
        .then((response) => (this.form = response.data))
        .catch(function (error) {
          console.log(error);
        });
    },
  },
};
</script>
