<template>
  <div class="common-layout">
    <el-container class="container">
      <el-aside class="aside" :style="{ 'width': (isCollapse ? '64px' : '230px') }">
        <AppMenu :isCollapse="isCollapse" :logoSrc="logoSrc" />
      </el-aside>
      <el-container>
        <el-header class="header">
          <el-row>
            <el-col :span="1">
              <!-- el-icon size:number -->
              <el-button type="text" @click="toggle"><el-icon :size="25">
                  <fold />
                </el-icon></el-button>
            </el-col>
          </el-row>
        </el-header>
        <el-main class="main"><router-view /></el-main>
      </el-container>
    </el-container>
  </div>
</template>

<script lang="ts" setup>
import AppMenu from '@/components/AppMenu.vue';
import { ref } from 'vue'
import { Fold } from '@element-plus/icons-vue';
let isCollapse = ref(false)
let logoSrc = "./logo-black.svg"
const toggle = (): void => {
  isCollapse.value = !isCollapse.value
  if (isCollapse.value == true) {
    logoSrc = "./logo.png"
  } else {
    logoSrc = "./logo-black.svg"
  }
}
</script>

<style scoped lang="scss">
.container {
  height: 100vh;

  .aside {
    overflow: hidden;
    box-shadow: 2.8px 2.8px 2.2px rgba(0, 0, 0, 0.02),
      6.7px 6.7px 5.3px rgba(0, 0, 0, 0.028),
      12.5px 12.5px 10px rgba(0, 0, 0, 0.035),
      22.3px 22.3px 17.9px rgba(0, 0, 0, 0.042),
      41.8px 41.8px 33.4px rgba(0, 0, 0, 0.05),
      100px 100px 80px rgba(0, 0, 0, 0.07);
  }

  .header {
    background: #ebf1f5;
    height: 64px;
    box-shadow:
      8.7px 9.2px 2.6px rgba(0, 0, 0, 0.023),
      18.5px 19.6px 6.3px rgba(0, 0, 0, 0.025),
      29.9px 31.7px 11.9px rgba(0, 0, 0, 0.024),
      44.2px 46.8px 21.2px rgba(0, 0, 0, 0.023),
      64.1px 67.9px 39.7px rgba(0, 0, 0, 0.028),
      100px 106px 95px rgba(0, 0, 0, 0.07);

  }

  .main {}
}
</style>