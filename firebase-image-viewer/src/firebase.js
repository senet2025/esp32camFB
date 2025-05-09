// Import Firebase
import { initializeApp } from "firebase/app";
import { getDatabase, ref, onValue } from "firebase/database";

// Firebase 설정 객체 (Firebase Console에서 복사한 설정을 여기에 붙여넣으세요)
const firebaseConfig = {
    apiKey: "AIzaSyBEhWh_6JB_jzxh1j9qqoZJQU8P9glQjMU",
    authDomain: "esp32-cam-5e283.firebaseapp.com",
    databaseURL: "https://esp32-cam-5e283-default-rtdb.firebaseio.com",
    projectId: "esp32-cam-5e283",
    storageBucket: "esp32-cam-5e283.firebasestorage.app",
    messagingSenderId: "296256185916",
    appId: "1:296256185916:web:1b609624bc7522c1400539",
    measurementId: "G-JG4KW1Y7WX"
  };

// Firebase 초기화
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

export { database, ref, onValue };