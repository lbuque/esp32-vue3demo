// /src/router/router.ts
// 引入 vue-router
import { createRouter, createWebHistory } from 'vue-router'
import AppLayout from '@/layout/index.vue'

const routes = [
  {
    path: '/',
    name: 'Status',
    redirect: '/status',
    component: AppLayout,
    children: [
      {
        path: '/status',
        name: 'Status',
        component: () => import('@/views/Status.vue')
      },
      {
        path: '/settings',
        name: 'Settings',
        component: () => import('@/views/Settings.vue')
      },
      {
        path: '/upgrade',
        name: 'Upgrade',
        component: () => import('@/views/Upgrade.vue')
      }
    ]
  },
  {
    path: '/settings',
    component: AppLayout,
    redirect: 'settings',
    children: [
      {
        path: 'settings',
        component: () => import('@/views/Settings.vue')
      }
    ]

  }
]
// createRouter创建路由
const router = createRouter({
  history: createWebHistory(),
  routes,
})
// 最后导出。es6的模块化方式
export default router
