import * as React from 'react';

import { backend_endpoint, game_engine_name as engine_name, game_engine_fps as engine_fps, client, retry_max_times } from '../../utils/client';
import { IDrawApiParam, IPrimitive } from '../../utils/types';
import drawFuncs from '../../utils/drawFuncs';

import './index.css'

type ControlKey = 'ArrowUp' | 'ArrowDown' | 'ArrowLeft' | 'ArrowRight' | 'Space';
type Direction = 'up' | 'down' | 'left' | 'right' | undefined;

interface IChess {
  type: 'empty' | 'wire' | 'router';
  owner?: number;
}

function GenerateChessboard(): IChess[][] {
  const emptyBoard: IChess[][] = Array.from({ length: 6 }, () => Array.from({ length: 8 }, () => ({ type: 'empty' })));
  emptyBoard[0][3] = { type: 'router', owner: 0 };
  emptyBoard[0][4] = { type: 'router', owner: 3 };
  emptyBoard[0][7] = { type: 'router', owner: 1 };
  emptyBoard[1][2] = { type: 'router', owner: 2 };
  emptyBoard[1][3] = { type: 'router', owner: 3 };
  emptyBoard[2][2] = { type: 'router', owner: 1 };
  emptyBoard[3][0] = { type: 'router', owner: 0 };
  emptyBoard[3][5] = { type: 'router', owner: 2 };
  return emptyBoard;
}

export default function Game() {
  const canvasRef = React.useRef<HTMLCanvasElement>(null);

  /* 处理键盘事件 */
  const [pressedKeys, setPressedKeys] = React.useState<ControlKey[]>([]);
  React.useEffect(() => {
    document.getElementById('pressed-key-list')!.scrollTo({
      left: document.getElementById('pressed-key-list')!.scrollWidth,
      behavior: 'smooth',
    });
  }, [pressedKeys])
  const handleKeyPressed = (e: React.KeyboardEvent<HTMLElement>) => {
    const extendKeyMap: { [key: string]: ControlKey } = {
      w: 'ArrowUp',
      a: 'ArrowLeft',
      s: 'ArrowDown',
      d: 'ArrowRight',
      ' ': 'Space',
    }
    if (e.key === 'ArrowUp' || e.key === 'ArrowDown' || e.key === 'ArrowLeft' || e.key === 'ArrowRight') {
      e.preventDefault();
      handleGameKey(e.key as ControlKey);
    }
    if (Object.keys(extendKeyMap).includes(e.key)) {
      e.preventDefault();
      handleGameKey(extendKeyMap[e.key]);
    }
  }
  const handleGameKey = (key: ControlKey) => {
    const directionMap: { [key in ControlKey]: Direction } = {
      'ArrowUp': 'up',
      'ArrowDown': 'down',
      'ArrowLeft': 'left',
      'ArrowRight': 'right',
      'Space': undefined,
    };
    setPressedKeys([...pressedKeys, key]);
    if (key === 'Space') handleSelect();
    else handleMove(directionMap[key]);
  }

  /* 连接后端 */
  const [, setLoading] = React.useState(true);
  const loadingRef = React.useRef(true);
  const fpsRef = React.useRef(0);
  const [fps, setFps] = React.useState(0);
  const first = React.useRef(false);
  const [start, setStart] = React.useState(true);
  const startTimeoutRef = React.useRef<number | null>(null);
  const [errorMessage, setErrorMessage] = React.useState('等待后端响应...');
  const [, setPrimitives] = React.useState<IPrimitive[]>([]);
  const [showingPrimitive, setShowingPrimitive] = React.useState<IPrimitive | null>(null);
  const fetchPrimitives = (update_index?: number) => {
    client("/engine/primitive/get_all").then(r => {
      const raw_primitives = r.data as { [apiEndpoint: string]: { [param: string]: object } }[];
      console.log(raw_primitives);
      const primitives = raw_primitives.map((raw_primitive, index) => {
        const apiEndpoint = Object.keys(raw_primitive)[0];
        const matchedDrawFuncs = drawFuncs.filter(df => df.drawFunc.apiEndpoint === apiEndpoint).map(df => ({
          params: df.drawFunc.params,
          type: df.drawFunc.type,
          names: df.drawFunc.params.map((param, index) => {
            if (param.type === 'point')
              return param.name || `p${index + 1}`;
            else if (param.type === 'func' && param.func)
              return param.name || `f${index + 1}`;
            else if (param.type === 'multi_points')
              return param.name || `mp${index + 1}`;
            else if (param.type === 'knots')
              return param.name || `knots`;
            else return param.name || `u${index + 1}`;
          }),
          algorithm: df.drawFunc.algorithm,
        })).sort((a, b) => {
          const matchTimes = (df: {
            params: IDrawApiParam[];
            names: string[];
          }) => df.names.filter(name => Object.keys(raw_primitive[apiEndpoint]).includes(name)).length;
          return matchTimes(b) - matchTimes(a);
        });
        const matchedDrawFunc = matchedDrawFuncs.length > 0 ? matchedDrawFuncs[0] : null;
        const params:
          (IDrawApiParam & { value: object })[]
          = matchedDrawFunc?.params.map((param, index) => {
            const name = (() => {
              if (param.type === 'point')
                return param.name || `p${index + 1}`;
              else if (param.type === 'func' && param.func)
                return param.name || `f${index + 1}`;
              else if (param.type === 'multi_points')
                return param.name || `mp${index + 1}`;
              else if (param.type === 'knots')
                return param.name || `knots`;
              else return param.name || `u${index + 1}`;
            })();
            return {
              type: param.type,
              name,
              value: raw_primitive[apiEndpoint][name],
            }
          }) || Object.keys(raw_primitive[apiEndpoint]).map((name) => ({
            type: 'unknown',
            name,
            value: raw_primitive[apiEndpoint][name],
          }))
        return { apiEndpoint, params, index, type: matchedDrawFunc?.type || 'unknown', editable: matchedDrawFunc ? true : false, algorithm: matchedDrawFunc?.algorithm || 0 };
      });
      console.log(primitives);
      setPrimitives(primitives);
      if (showingPrimitive && update_index) {
        const newShowingPrimitive = primitives.find(p => p.index === update_index);
        if (newShowingPrimitive) setShowingPrimitive(newShowingPrimitive);
      }
    })
  }
  React.useEffect(() => {
    if (!first.current) return;
    first.current = false;

    const width = 800;
    const height = 600;

    startTimeoutRef.current = setTimeout(() => {
      setStart(false);
    }, 100);

    const ConnectToBackend = (retryTimes = 0) => new Promise((resolve, reject) => {
      client('/engine/create', {
        data: {
          name: engine_name,
          width,
          height,
        },
        timeout: 1000,
      }).then(r => {
        console.log(r.data);
        console.log(`Engine ${engine_name} created with ${width}x${height}`);
        setInterval(() => {
          setFps(fpsRef.current);
          if (fpsRef.current === 0) {
            window.location.reload();
          }
        }, 500);
        fetchPrimitives();
        resolve(1);
      }).catch(e => {
        console.error(e);
        setErrorMessage(`后端无响应，重新尝试连接.. ${retryTimes + 1}/${retry_max_times}`);
        if (retryTimes >= retry_max_times) {
          reject('Failed to create engine');
          return;
        } else {
          setTimeout(() => {
            ConnectToBackend(retryTimes + 1).then(resolve).catch(reject);
          }, 1000)
        }
      });
    });

    ConnectToBackend().then(() => {
      const ws = new WebSocket(`//${backend_endpoint}/engine/${engine_name}`);
      ws.onopen = function () {
        ws.send(`set_fps ${engine_fps}`);
        setLoading(false);
        if (startTimeoutRef.current) clearTimeout(startTimeoutRef.current);
        setStart(true);
        loadingRef.current = false;
      };
      ws.binaryType = "arraybuffer";
      ws.onmessage = function (event) {
        if (!canvasRef.current) return;

        fpsRef.current += 1;
        setTimeout(() => {
          fpsRef.current -= 1;
        }, 1000)

        const ctx = canvasRef.current.getContext("2d");
        canvasRef.current.width = width;
        canvasRef.current.height = height;
        const data = new Uint8ClampedArray(event.data);

        const imageData = new ImageData(data, width, height);
        if (ctx)
          ctx.putImageData(imageData, 0, 0);
      };
    }).catch(e => {
      console.error(e);
      setErrorMessage("后端无响应，请检查后端是否正常运行。");
    });
  }, [])

  /* 游戏逻辑 */
  const [chessboard, setChessboard] = React.useState<IChess[][]>(GenerateChessboard());
  React.useEffect(() => {
    console.log('chessboard', chessboard);
  }, [chessboard])
  const [currentPosition, setCurrentPosition] = React.useState({ x: 0, y: 0 });
  React.useEffect(() => {
    console.log('position', currentPosition);
  }, [currentPosition])
  const handleMove = (direction: Direction) => {
    const putWire = (x: number, y: number) => {
      if (currentOwner < 0) {
        setCurrentPosition({ x, y });
        return;
      } else {
        if (chessboard[y][x].type === 'empty') {
          // 空的地方放线
          chessboard[y][x] = { type: 'wire', owner: currentOwner };
          setCurrentPosition({ x, y });
        }
        else if (chessboard[y][x].type === 'wire') {
          // 线不能交叉
          return;
        } else if (chessboard[y][x].type === 'router') {
          // 不是自己的路由器不能进
          if (chessboard[y][x].owner !== currentOwner || currentStartChess && y === currentStartChess.y && x === currentStartChess.x)
            return;
          else {
            // 成功连线
            setCurrentPosition({ x, y });
            setCurrentOwner(-1);
            setCurrentStartChess(null);
            checkWin();
          }
        }
      }
    }
    if (direction === 'up') {
      if (currentPosition.y > 0) putWire(currentPosition.x, currentPosition.y - 1);
    }
    if (direction === 'down') {
      if (currentPosition.y < 5) putWire(currentPosition.x, currentPosition.y + 1);
    }
    if (direction === 'left') {
      if (currentPosition.x > 0) putWire(currentPosition.x - 1, currentPosition.y);
    }
    if (direction === 'right') {
      if (currentPosition.x < 7) putWire(currentPosition.x + 1, currentPosition.y);
    }
  }
  const [currentOwner, setCurrentOwner] = React.useState(-1);
  const [currentStartChess, setCurrentStartChess] = React.useState<{ x: number, y: number } | null>(null);
  const handleSelect = () => {
    const currentChess = chessboard[currentPosition.y][currentPosition.x];
    console.log('handleSelect', currentChess);
    if (currentChess.type === 'router') {
      const owner = currentChess.owner === undefined ? -1 : currentChess.owner;
      if (owner === currentOwner)
        setCurrentOwner(-1);
      else {
        setCurrentOwner(owner);
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === owner ? { type: 'empty' } : chess)));
        setCurrentStartChess({ x: currentPosition.x, y: currentPosition.y });
      }
    } else if (currentChess.type === 'wire') {
      if (currentOwner === -1) {
        const owner = currentChess.owner === undefined ? -1 : currentChess.owner;
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === owner ? { type: 'empty' } : chess)));
      } else {
        setCurrentOwner(-1);
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === currentOwner ? { type: 'empty' } : chess)));
        setCurrentStartChess(null);
      }
    }
  }
  const checkWin = () => {
    const owners = new Set<number>();
    chessboard.forEach(row => row.forEach(chess => {
      if (chess.type === 'router') owners.add(chess.owner || -1);
    }));
    const connectedOwners = new Set<number>();
    chessboard.forEach(row => row.forEach(chess => {
      if (chess.type === 'wire') connectedOwners.add(chess.owner || -1);
    }));
    if (owners.size === connectedOwners.size) {
      alert('You Win!');
    }
  }


  return <div id="game">
    <div id="hover" style={{ opacity: start ? 0 : 1 }}>
      <div>{errorMessage}</div>
    </div>
    <div id="mousePosition">
      Engine: {engine_name};
      FPS: {fps}.</div>
    <p id='title'>
      <span style={{ color: "#2ecc71" }}>断</span>
      <span style={{ color: "#f1c40f" }}>线</span>
      <span style={{ color: "#74b9ff" }}>重</span>
      <span style={{ color: "#e74c3c" }}>连</span>
    </p>
    <div
      id='hide'
      onKeyDown={handleKeyPressed}
      tabIndex={0}
    >
      {
        chessboard.map((row, y) => row.map((chess, x) => {
          return <div key={`${x}-${y}`} className={`chess ${chess.type} ${chess.owner === undefined ? -2 : chess.owner}`} style={{ top: y * 100, left: x * 100 }} />
        }))
      }
      <div className={`chess current ${currentOwner}`} style={{ top: currentPosition.y * 100, left: currentPosition.x * 100 }} />
    </div>
    <canvas
      width={800}
      height={600}
      ref={canvasRef}
    />
    <div id='pressed-key-list'>
      {
        pressedKeys.map((key, index) => <div key={index} className={`key-icon ${key}`} />)
      }
    </div>
  </div>
}