// ====================================================================
//  Service worker de Retro Pixel LED (control)
// ====================================================================
const PWA_VERSION  = "1.0.0";
const CACHE_PREFIX = "retropixel-shell-v";
const CACHE_NAME   = CACHE_PREFIX + PWA_VERSION;

const APP_SHELL = [
  "./",
  "./index.htm",
  "./manifest.json",
  "./icons/icon-192.png",
  "./icons/icon-512.png"
];

// URLs completas del shell (relativas a la carpeta donde vive este sw.js)
const SHELL_URLS = new Set(APP_SHELL.map((p) => new URL(p, self.registration.scope).href));

self.addEventListener("install", (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) =>
      // Un archivo que falle (por ejemplo un 404) ya no impide instalar el resto.
      // "reload" evita copiar una versión vieja de la caché HTTP del navegador.
      Promise.allSettled(APP_SHELL.map((u) => cache.add(new Request(u, { cache: "reload" }))))
    )
  );
  self.skipWaiting();
});

self.addEventListener("activate", (event) => {
  event.waitUntil(
    caches.keys().then((keys) =>
      Promise.all(
        keys
          // Solo borramos NUESTRAS cachés antiguas
          .filter((k) => k.startsWith(CACHE_PREFIX) && k !== CACHE_NAME)
          .map((k) => caches.delete(k))
      )
    )
  );
  self.clients.claim();
});

self.addEventListener("fetch", (event) => {
  // Solo servimos desde caché el "shell" estático de la app. Todo lo demás
  // (peticiones al panel, version.json, POST...) lo gestiona el navegador sin pasar por aquí.
  if (event.request.method !== "GET") return;
  if (!SHELL_URLS.has(event.request.url)) return;

  event.respondWith(
    caches.match(event.request).then((cached) => cached || fetch(event.request))
  );
});
