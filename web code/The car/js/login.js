// Initialize Firebase
const firebaseConfig = {
  apiKey: "AIzaSyBhrGH0xtRk6Uq2XViXfYpP1foPb2SC8Zc",
  authDomain: "car-robot-fd300.firebaseapp.com",
  databaseURL: "https://car-robot-fd300-default-rtdb.firebaseio.com",
  projectId: "car-robot-fd300",
  storageBucket: "car-robot-fd300.appspot.com",
  messagingSenderId: "461609510154",
  appId: "1:461609510154:web:b191a9e61fdb2e7b4ec6f3",
  measurementId: "G-3QX0JJQHTY"
  
  };
 firebase.initializeApp(firebaseConfig);
 
 
  // Login form submit event
  document.getElementById("loginForm").addEventListener("submit", function (e) {
    e.preventDefault(); // Prevent form submission
  
    var email = document.getElementById("loginEmail").value;
    var password = document.getElementById("loginPassword").value;
  
    // Sign in with email and password
    firebase.auth().signInWithEmailAndPassword(email, password)
      .then(function (userCredential) {
        var user = userCredential.user;
        // Get user email
        var userEmail = user.email;
        
        // Redirect to home page and display the user email
        window.location.href = "home.html?email=" + userEmail;
      })
      .catch(function (error) {
        const errorMessage = error.message;
        // Display error message to the user or perform any error handling
        console.log('Registration error:', errorMessage);
        alert(error);
      });
  });
  
  
  
  
  document.getElementById("registerForm").addEventListener("submit", function (e) {
    e.preventDefault(); // Prevent form submission
  
    var email = document.getElementById("registerEmail").value;
    var password = document.getElementById("registerPassword").value;
  
    // Sign in with email and password
    firebase.auth().createUserWithEmailAndPassword(email, password)
      .then(function (userCredential) {
        var user = userCredential.user;
        // Get user email
        var userEmail = user.email;
        console.log('User registered:', user);
        alert("Registration successfully!!");
        // Redirect to home page and display the user email
        window.location.href = "home.html?email=" + userEmail;
      })
      .catch(function (error) {
        const errorCode = error.code;
        const errorMessage = error.message;
        // Display error message to the user or perform any error handling
        console.log('Registration error:', errorMessage);
        alert(error);
      });
  });
  
  function loginWithGoogle() {
    const provider = new firebase.auth.GoogleAuthProvider();
    
    firebase.auth()
    .signInWithPopup(provider)
    .then(function(result) {
    // This gives you a Google Access Token. You can use it to access the Google API.
    const token = result.credential.accessToken;
    // The signed-in user info.
    const user = result.user;
    
    var userEmail = user.email;
    alert("Registration successfully!!");
    // Redirect to home page and display the user email
    window.location.href = "home.html?email=" + userEmail;
    
    })
    .catch(function(error) {
    // Handle Errors here.
    const errorCode = error.code;
    const errorMessage = error.message;
    console.error("Error:", errorCode, errorMessage);
   
    });
    }

   
  // Your Firebase configuration object goes here` with your actual Firebase configuration obtained from the Firebase Console.


