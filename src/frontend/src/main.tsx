import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import {
  RouterProvider,
  createBrowserRouter
} from 'react-router-dom'

import App from "./pages/index";
import Game from "./pages/game";

// eslint-disable-next-line react-refresh/only-export-components
export const ROUTERS = createBrowserRouter([
  {
    path: "/",
    element: <App />,
  },
  {
    path: "/game",
    element: <Game />,
  },
]);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <RouterProvider router={ROUTERS} />
  </StrictMode>,
)
