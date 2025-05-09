import React, { useEffect, useState } from "react";
import { database, ref, onValue } from "./firebase";

function App() {
  const [imageData, setImageData] = useState(null);

  useEffect(() => {
    // Firebase Realtime Database에서 데이터 읽기
    const dbRef = ref(database, "/JSON/ESP32CAM");
    onValue(dbRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        setImageData(data); // Base64 데이터 설정
      }
    });
  }, []);

  return (
    <div style={{ textAlign: "center", marginTop: "50px" }}>
      <h1>Firebase Image Viewer</h1>
      {imageData ? (
        <img
          src={imageData}
          alt="Captured"
          style={{ maxWidth: "100%", height: "auto", border: "1px solid #ccc" }}
        />
      ) : (
        <p>Loading image...</p>
      )}
    </div>
  );
}

export default App;
