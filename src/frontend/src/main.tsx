import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import {
  RouterProvider,
  createBrowserRouter,
  redirect
} from 'react-router-dom'

import PlayGround from "./pages/playground";
import Game from "./pages/game";

// eslint-disable-next-line react-refresh/only-export-components
export const ROUTERS = createBrowserRouter([
  {
    path: "/",
    loader: () => {
      return redirect("/playground");
    }
  },
  {
    path: "/playground",
    element: <PlayGround />,
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
